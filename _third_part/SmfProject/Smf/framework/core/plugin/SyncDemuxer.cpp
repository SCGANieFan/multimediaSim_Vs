#include "SyncDemuxer.h"
#include "IOCacheReader.h"
#include "smf_debug.h"
#include <string.h>
//
using namespace smf;
using namespace smf::audio;
//
SyncDemuxer::SyncDemuxer(){
}
SyncDemuxer::~SyncDemuxer() {
	Status(EStatus::null);
}
bool SyncDemuxer::open(void* param0) {
	returnIfErrC(false, !initIO());
	_hsize = 0;
	_hend = 0;
	_fsize = 0;
	_fcnt = 0;
	_fdura = 0;
	_parsed = false;
	returnIfErrE(false, !parseFileHeader(), SMF_ERROR_DEMUX_HEADER_FAIL);
	returnIfErrE(false, !headerParse(), SMF_ERROR_DEMUX_HEADER_FAIL);
	returnIfErrE(false, !Parse(), SMF_ERROR_DEMUX_HEADER_FAIL);
	auto duration = ((uint64_t)_fdura) * _fcnt / 1000;
	return init(_fcnt,duration);
}
//bool SyncDemuxer::close() {
//	dbgTestPL();
//	return IOSource::close();
//}

int SyncDemuxer::syncFrame(uint8_t*& ptr, uint8_t* end) {
	while (ptr < end) {
		auto fsize = checkFrameHeader(ptr);
		if (fsize) {
			if (!_parsed) {
				return fsize;
			}
			if (compareFrameHeader(ptr, _frameHeader)) {
				return fsize;
			}
		}
		ptr++;
	}
	return 0;
}
int SyncDemuxer::syncFrame(int rcnt0,uint8_t* frame) {
	auto rcnt = rcnt0;	
	auto _ioReader = static_cast<IOCacheReader*>(_io);
	while (rcnt--) {
		int size = 0;
		uint8_t* data = _ioReader->Peek(size);
		if (!size) {//read from io and fill cache
			returnIfInfoE(0, !_ioReader->SyncFill(), SMF_ERROR_IO_EOS);
			data = _ioReader->Peek(size);
			returnIfInfoE(0, !data, SMF_ERROR_IO_EOS);
			returnIfInfoE(0, !size, SMF_ERROR_IO_EOS);
		}
		if (size < _frameHeadLen) {//sync frame for left data
			memcpy(_buffsync, data, size);
			data = _buffsync;
			_ioReader->Skip();///skip all
			returnIfInfoE(0, !_ioReader->SyncFill(), SMF_ERROR_IO_EOS);
			auto rs = _ioReader->Peek(data + size, _frameHeadLen - 1);
			returnIfInfoE(0, !rs, SMF_ERROR_IO_EOS);
			returnIfInfoE(0, rs != _frameHeadLen - 1, SMF_ERROR_IO_EOS);
			auto fsize = syncFrame(data, data + size);
			if (fsize) {
				_syncLeft = size - (data - _buffsync);
				_frame = data;
				if (frame) {
					memcpy(frame, _frame, _frameHeadLen);
					_frame = frame;
				}
				return fsize;
			}
		}
		else {
			auto data0 = data;
			auto fsize = syncFrame(data, data + size - (_frameHeadLen - 1));
			_ioReader->Skip(data - data0);
			if (fsize) {
				_syncLeft = 0;
				_frame = data;
				if (frame) {
					memcpy(frame, _frame, _frameHeadLen);
					_frame = frame;
				}
				return fsize;
			}

		}
	}
	//dbgErrPEL(SMF_ERROR_DEMUX_SYNC_FAIL, "sync frame fail[%d]%d,%p", _outsts.frameindex,  rcnt0, _io->GetOffset() - (_end - _ptr));
	//dbgErrPEL(SMF_ERROR_DEMUX_SYNC_FAIL, "sync frame fail[%d]%d,%p", _outsts.frameindex,  rcnt0, _io->GetOffset());
	return 0;
}
bool SyncDemuxer::headerParse() {
	returnIfErrC(false, !_frameHeadLen);
	//returnIfErrC(false, !scan());
	uint8_t frameBuff[64];
	uint8_t* frame0 = frameBuff;
	uint8_t* frame1 = frameBuff + _frameHeadLen;
	uint8_t* frame2 = frame1 + _frameHeadLen;
	int fsize = 0;
	for (int i = 0; i < 8;i++) {
		int fs0 = syncFrame((i == 0 ? 1280 : 8), frame0);
		returnIfErrC(false, !fs0);
		auto pos0 = _io->GetOffset() - _syncLeft;
		int pos1 = pos0 + fs0;
		//
		returnIfErrC(false, !_io->Seek(pos1, Position::front));
		int fs1 = syncFrame(8, frame1);
		returnIfErrC(false, !fs1);
		//
		if (compareFrameHeader(frame0, frame1)) {
			int pos2 = pos1 + fs1;
			returnIfErrC(false, !_io->Seek(pos2, Position::front));
			int fs2 = syncFrame(8, frame2);
			returnIfErrC(false, !fs2);
			//dbgInfoDump(frame2, 8);
			if (compareFrameHeader(frame1, frame2)) {
				returnIfErrC(false, !_io->Seek(pos0, Position::front));
				_syncLeft = 0;
				fsize = fs0;
				break;
			}
		}
		else {
			_syncLeft = 0;
			returnIfErrC(false, !_io->Seek(pos0+1, Position::front));
		}
	}
	returnIfErrCS(false, !fsize, "parse header fail");
	return true;
}
bool SyncDemuxer::generateFrame(Frame*&frm) {
	if(_fsize && frm->size>=_fsize){
		return true;
	}
	int fsize = syncFrame(4);
	if (!fsize) {
		if (_progress) {
			_progress->current = _io->GetOffset() - _hsize;
			dbgTestPXL("index %d pos %d",frm->index, _progress->current);
		}
		// returnIfNoteC(false, !fsize);
		returnIfErrC(false, !fsize);
	}
	int memleft = frm->max - frm->offset - frm->size;
	if (fsize > memleft) {
		dbgWarnPXL("%d>%d(%d,%d,%d)%d,%d", fsize, memleft, frm->max, frm->offset, frm->size, _oport->_min, _oport->_max);
		if (frm->offset) {
			memmove(frm->buff, frm->Data(), frm->size);
			frm->offset = 0;
			return false;
		}
		else if (frm->size) {
			return true;
		}
		else{
			_oport->SetReallocFrame(fsize);
			return false;
		}
	}
	//returnIfTestES(false, fsize > memleft, SMF_ERROR_OUTPUT_BUFFER_NOT_ENOUGH
	//	, "%d>%d(%d,%d,%d)%d,%d"
	//	, fsize,memleft, frm->max , frm->offset , frm->size, _oport->_min,_oport->_max);
	unsigned char* data = (unsigned char*)frm->buff + frm->offset + frm->size;
	int size = fsize;
	if (_syncLeft) {
		memcpy(data, _frame, _syncLeft);		
		data += _syncLeft;
		size -= _syncLeft;
		_syncLeft = 0;
	}
	if (size) {
		auto rsize = _io->Read(data, size);
		returnIfErrE(false, !rsize, SMF_ERROR_IO_EOS);
		returnIfErrE(false, (int)rsize < size, SMF_ERROR_IO_EOS);
	}
	if (!frm->TimeStampBase()) {
		frm->TimeStampReset();
	}
	frm->size += fsize;
	frm->timestamp  = _fdura * _frameIndex/1000;
	frm->index  = _frameIndex++;
	return update(frm);
}

bool SyncDemuxer::Parse() {
	//returnIfErrC(false, !_io->Seek(_hsize, Position::front));
	auto fcnt = 0;
	auto fmax = 0;
	auto total = _io->GetSize();
	auto hsize = _frameHeadLen;
	SmfBuffer buff(4 * 8 + hsize *2);
	auto offs = buff.get<uint32_t>();
	memset(offs, 0, 4 * 8 + hsize *2);
	auto ptr0 = (char*)offs + 4 * 8;//buff.get<char>();
	auto ptr1 = ptr0 + hsize;
	auto sum = 0;
	auto end = total - _hend;
	bool parsed = false;
	char* buffs[2]{ ptr0, ptr1 };
	auto cmpc = 0;
	int fsize = 0;
	while (sum < end) {
		auto ptr = buffs[fcnt & 1];
		offs[fcnt & 3] = _io->GetOffset();
		auto rsize = _io->Read(ptr, hsize);
		if (rsize != hsize)
			break;
		fsize = checkFrameHeader(ptr);
		if (!fsize)
			break;
		if (fmax < fsize)
			fmax = fsize;
		if (!parsed) {
			if (compareFrameHeader(ptr0, ptr1)) {
				cmpc++;
			}
			else {
				cmpc = 0;
			}
			if (cmpc >= _checkFrameCount) {
				parsed = parseFrameHeader(ptr0);
				if (parsed) {
					memcpy(_frameHeader, ptr0, hsize);
					dbgTestPXL("%u,%u,%u,%u==%u", cmpc, fcnt, _checkFrameCount, _hsize, offs[(fcnt - _checkFrameCount) & 3]);
					_hsize = offs[(fcnt - _checkFrameCount) & 3];
					if (!_scan || _fcnt) {
						break;
					}
				}
			}
		}
		// dbgTestPXL("%u,%u,%u",fcnt,fsize, sum);
		fcnt++;
		sum += fsize;
		returnIfErrE(false, !_io->Seek(fsize - hsize, Position::current), SMF_ERROR_IO_SEEK_FAIL);
	}
	_parsed = parsed;
	returnIfErrE(false, !parsed, SMF_ERROR_DEMUX_PARSE_FRAME_FAIL);
	//
	if (_fcnt) {
		fcnt = _fcnt;
		fmax = _oport->Media()->FrameMax();
	}
	else if (!_scan) {
		auto tsize = total - _hsize - _hend;
		fcnt = tsize * fcnt / sum;
		fmax = _oport->Media()->FrameMax();
	}
	//
	auto fdura = _oport->Media()->FrameDurationUs();
	returnIfErrC(false, !fdura);
	_oport->_max = fmax;
	_fsize = fsize;
	_fcnt = fcnt;
	_fdura = fdura;
	dbgTestPXL("%u,%u,%u", _fsize, _fcnt, _fdura);
	return true;
}

bool SyncDemuxer::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("scan"):_scan = (bool)val; return true;
	case Hash("checkFrameCount"):_checkFrameCount = (uint32_t)val; return true;
	}
	return IOSource::set(key, val);
}

bool SyncDemuxer::seekToIndex(uint32_t frameindex) {
	_frameIndex = frameindex;
	auto posi = _hsize + _fsize * frameindex;
	dbgTestPXL("[%u]%u", frameindex, posi);
	return _io->Seek(posi, Position::front);
}
