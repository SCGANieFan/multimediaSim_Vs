#include "IOSource.h"
#include "Source.h"
#include "SmfHash.h"
#include "SmfMath.h"
#include "MediaAudio.h"
#include "smf_debug.h"
#include "smf_muxer.h"
#include "IOCacheReader.h"
#include "unique_ptr.h"
using namespace smf;
///
EXTERNC void smf_io_source_register() {
	IOSource::Register<IOSourceNormal>("src-io");
}
///
IOSource::IOSource() {
}
IOSource::~IOSource() {
	Status(EStatus::null);
	if (_io)delete _io;
}
/// 
bool IOSource::open(void*) {
	returnIfErrC(false, !initIO());
	return true;
}
bool IOSource::close() {
	if (_io) { _io->Close(); delete _io; _io = 0; }
	_extra.reset();
	_title.reset();
	_artist.reset();
	_album.reset();
	return true;
}

bool IOSource::initIO() {
	returnIf(true, _io);
	_io = _url.Create("rb");
	returnIfErrE(false, !_io, SMF_ERROR_IO_OPEN_FAIL);
	_io->Parent(this);
	//dbgTestPDL(_ioCacheLen);
	if (_ioCacheLen) {
		auto reader = new IOCacheReader();
		returnIfErrE(false, !reader, SMF_ERROR_POOL_ALLOC_NULL);
		unique_ptr<IO> iox(reader);
		reader->Parent(this);
		IOCacheReader::OpenParam op{ _io,_ioCacheLen, _ioCacheAlign, true };
		_io = 0;
		returnIfErrE(false, !reader->Open(&op), SMF_ERROR_IO_FAIL);
		_io = reader;
		iox.release();
	}
	return true;
}

bool IOSource::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("seekForMs"): SeekFor(*(duration32_t*)&val); return true;
	case Hash("seekToMs"): SeekTo(*(timepoint32_t*)&val); return true;
	case Hash("startPosition"): SetStartPosition((uint32_t)val); return true;
	case Hash("startMs"): SetStartTimepoint((uint32_t)val); return true;
	case Hash("startIndex"): SetStartIndex((uint32_t)val); return true;
	case Hash("startPercentage"): SetStartPercent((uint32_t)val); return true;
	case Hash("io"): _io = (IO*)val; return true;
	case Hash("cache"): _ioCacheLen = (int)val; return true;
	case Hash("url"): _url.Parse((const char*)val); return true;
	case Hash("iopara"): _url._other = val; return true;
	case Hash("meta"): _meta = val ? (smf_meta_info_t*)val : &_metaX; return true;
	case Hash("progress"): _progress = (smf_progress_t*)val; return true;
	case Hash("url_extra"):initExtra((char*)val); return true;
	case Hash("loopPlay"):_flagsExt.Set(eLoopPlay, (bool)val); return true;
	default:break;
	}
	return Source::set(key, val);
}

bool IOSource::get(uint32_t key, void* val) const {
	switch (key) {
		//case Hash("progress"): *((smf_progress_t**)val)= (smf_progress_t*)&_progress; return true;
		//case Hash("pos"): {
		//	if(val) {
		//		smf_message_t msg;
		//		memset(&msg, 0, sizeof(msg));
		//		msg.id = fcc64("pos");
		//		msg.direction = smf_direction_forward;
		//		msg.creater = (void*)this;
		//		((IOSource*)this)-> SendMessage(msg);
		//		//dbgTestPDL(msg.data[0]);
		//		//*(uint32_t*)val = _startPostion + msg.data[0];
		//		*(uint32_t*)val = _startPostion + msg.data[0];
		//	}
		//	return true;
		//}
	case Hash("progress"):
	case Hash("timepoint"):	*(region_t**)val = (region_t*)&_timepoint;  return true;
	case Hash("frameindex"):*(region_t**)val = (region_t*)&_frameindex; return true;
	case Hash("meta"):*(smf_meta_info_t**)val = _meta; return true;
	case Hash("progressV"): 
	case Hash("timepointV"): *(region_t*)val = _timepoint; return true;
	case Hash("metaV"): return fill((smf_meta_info_t*)val);
	default: return Source::get(key, val);
	}
}
bool IOSource::setParam(void*para0) {
	auto para = (smf_iosource_param_t*)para0;
	returnIfErrC(false, !para);
	//returnIfErrC(false, para->size!=sizeof(smf_iosource_param_t));
	para->media = (smf_media_t*)_oport->Media()->ToMedia();

	if(para->url)_url.Parse(para->url);
	if (para->io)_io = (IO*)para->io;
	if (para->ioReadCacheLen)_ioCacheLen = para->ioReadCacheLen;
	if (para->ioReadAlign)_ioCacheAlign = para->ioReadAlign;
	//_packed = para->packed;
	if(para->progress)_progress = para->progress;
	if(para->meta)_meta = para->meta;
	return true;
}

bool IOSource::initExtra(const char* url) {
	URL urlx(url);
	returnIfErrC(false, !urlx.Read(_extra));
	auto media = oMedia();
	if (media) {
		media->_extraData = (uint8_t*)_extra.get();
		media->_extraSize = _extra.size();
	}
	return true;
}

bool IOSource::init(uint32_t framecount, uint32_t duration_ms) {
	auto media = oMedia();
	returnIfErrC(false, !media);
	//
	_timepoint.min = 0;
	_timepoint.max = duration_ms;
	_timepoint.current = 0;
	_frameindex.min = 0;
	_frameindex.max = framecount;
	_frameindex.current = 0;
	// dbgTestPXL("%d,%d", duration_ms, framecount);
	//
	if (_start.val) {
		switch (_eStart) {
		case eStart::position:break;// returnIfErrC(false, !seek(_start.position)); break;
		case eStart::timepoint:
			//returnIfNoteCS(false, _start.timepoint.ms > duration_ms, "%u>=%u", _start.timepoint.ms, duration_ms);
			if (_start.timepoint.ms > duration_ms) {
				dbgWarnPXL("%u>=%u", _start.timepoint.ms , duration_ms);
				_start.timepoint.ms = duration_ms;
			}
			returnIfErrE(false, !seekToMs(_start.timepoint.ms), SMF_ERROR_IO_SEEK_FAIL); break;
		case eStart::index:
			//returnIfNoteCS(false, _start.index > framecount, "%u>=%u", _start.index, framecount);
			if (_start.index > framecount) {
				dbgWarnPXL("%u>=%u", _start.index, framecount);
				_start.index = framecount;
			}
			returnIfErrE(false, !seekToIndex(_start.index), SMF_ERROR_IO_SEEK_FAIL); break;
		case eStart::percent:
			//returnIfNoteC(false, _start.percent.val > 100, "%u>=%u", _start.percent.val, 100);
			if (_start.percent.val > 100) {
				dbgWarnPXL("%u>=%u", _start.percent.val, 100);
				_start.percent.val = 100;
			}
			returnIfErrE(false, !seekToMs(duration_ms * _start.percent.val / 100u), SMF_ERROR_IO_SEEK_FAIL); break;
		default:break;
		}
	}
	else {
		returnIfErrC(false, !seekToIndex(0));
	}
	//
	if (_extra) {
		media->_extraData = (uint8_t*)_extra.get();
		media->_extraSize = _extra.size();
	}
	//
	if (_meta) {
		fill(_meta);
	}
	//
	if (_progress) {
		*_progress = (smf_progress_t&)_timepoint;
	}
	//
	uint32_t ext32 = 0;
	if (media) {
		_sinfo.forward.is_audio = media->IsAudio();
		_sinfo.forward.is_video = media->IsVideo();
		if (_sinfo.forward.is_audio) {
			ext32 = 0x80000000 | (_sinfo.forward.audio.mute << 16) | _sinfo.forward.audio.volume;
		}
	}
	//dbgTestPXL("%u/%u,%u@%u",duration_ms,framecount, (uint32_t)_eStart, _start.val);
	SendMessage("media", smf_direction_upward, (uint32_t)oMedia(), (uint32_t)&_sinfo, (uint32_t)_meta, (uint32_t)ext32);
	returnIfErrE(false, media->IsError(), SMF_ERROR_DEMUX_MEDIA_PARAM_FAIL);
	return true;
}

bool IOSource::fill(smf_meta_info_t* meta) const {
	returnIfErrC(false, !meta);
	meta->url = _url.url();
	meta->media = (smf_media_def_t*)oMedia();
	meta->timepoint = (smf_progress_t*)&_timepoint;
	meta->frameindex = (smf_progress_t*)&_frameindex;
	meta->title.data = _title.get();
	meta->title.size = _title.size();
	meta->artist.data = _artist.get();
	meta->artist.size = _artist.size();
	meta->album.data = _album.get();
	meta->album.size = _album.size();
	return true;
}

bool IOSource::update(Frame*frm) {
	if (frm) {
		// dbgTestPXL("%d,%d", frm->timestamp, frm->index);
		_timepoint.current = frm->timestamp;
		_frameindex.current = frm->index;
		if (_progress) {
			_progress->current = _timepoint.current;
		}
	}
	//update seek
	if (_seekMs != 0xffffffff) {
		dbgTestPDL(_seekMs);
		returnIfErrE(false, !seekToMs(_seekMs), SMF_ERROR_IO_SEEK_FAIL);
		_seekMs = 0xffffffff;
	}
	if (_seekIndex != 0xffffffff) {
		dbgTestPDL(_seekIndex);
		returnIfErrE(false, !seekToIndex(_seekIndex), SMF_ERROR_IO_SEEK_FAIL);
		_seekIndex = 0xffffffff;
	}
	return frm;
}

void IOSource::SetStartPosition(uint32_t v) {
	_start.position = (uint32_t)v;
	_eStart = eStart::position;
}
void IOSource::SetStartTimepoint(uint32_t v) { 
	_start.timepoint.ms = (uint32_t)v;
	_eStart = eStart::timepoint; 
}
void IOSource::SetStartIndex(uint32_t v) { 
	_start.index = (uint32_t)v;
	_eStart = eStart::index; 
}
void IOSource::SetStartPercent(uint32_t v) { 
	_start.percent.val = (uint32_t)v;
	_eStart = eStart::percent;
}

bool IOSource::seekToIndex(uint32_t frameindex) {
	returnIfErrC(false, !_frameindex.max);
	uint32_t timepoint = (uint64_t)frameindex * _timepoint.max / _frameindex.max;
	dbgTestPXL("[%u]%u",frameindex, timepoint);
	return seekToMs(timepoint);
}

bool IOSource::seekToMs(uint32_t timepoint) {
	returnIfErrC(false, !_timepoint.max);
	uint32_t frameindex = (uint64_t)timepoint * _frameindex.max / _timepoint.max;
	dbgTestPXL("[%u]%u", frameindex, timepoint);
	return seekToIndex(frameindex);
}

bool IOSource::SeekTo(uint32_t frameindex) {	
	_seekIndex = Clamp(frameindex, _frameindex.min, _frameindex.max);
	return true;
}
bool IOSource::SeekTo(const timepoint32_t& tp) {
	_seekMs = Clamp(tp.ms, _timepoint.min, _timepoint.max);
	return true;
}
bool IOSource::SeekTo(const percentage_t& percent) {
	returnIfErrC(false, !_timepoint.max);
	// DEBUG: need use SeekTo(const timepoint32_t& tp) overload version
	return SeekTo( _timepoint.max * percent.val / 100ul);
}
bool IOSource::SeekFor(int32_t frameindex) {
	return SeekTo(Clamp(frameindex + _frameindex.current, _frameindex.min, _frameindex.max));
}
bool IOSource::SeekFor(const duration32_t& offset) {
	int offset_ms = offset.ms + (int)_timepoint.current;
	uint32_t offset_time = 0;
	if( offset_ms < 0){
		offset_time = 0;
	}else{
		offset_time = offset_ms;
	}
	return SeekTo(timepoint32_t{ Clamp(offset_time, _timepoint.min, _timepoint.max) });
}
bool IOSource::SeekFor(const percentage_t& percent) {
	returnIfErrC(false, !_timepoint.max);
	return SeekFor(duration32_t{ (int32_t)(_timepoint.max * percent.val / 100l) });
}
bool IOSource::processError(Frame* ifrm, Frame* ofrm) {
	switch (_error.errid) {
	case SMF_ERROR_IO_EOS:
		if (_flagsExt.Check(eLoopPlay)) {
			dbgTestPXL("LoopPlay");
			returnIfErrC(false, !SeekTo(0));
			if(ofrm)ofrm->flags &= ~SMF_FRAME_IS_EOS;
			update(ofrm);
			CleanError();
			return true;
		}
		break;
	default:
		break;
	}
	return Source::processError(ifrm, ofrm);
}
char* IOSource::print(char* ptr, char* end) const {
	ptr = Source::print(ptr, end);
	ptr = snprintf(ptr, end, "%s(%u,%u/%u,%u/%u,%u-%u-%u-%u,%u"
		, _url.url(), _ioCacheLen
		, _frameindex.current, _frameindex.max
		, _timepoint.current, _timepoint.max
		, _extra.size(), _title.size(), _artist.size(), _album.size()
		, _flagsExt.Check(eLoopPlay)
	);
	return ptr;
}
//IOSourceFixedFrame
bool IOSourceFixedFrame::open(void*) {
	returnIfErrC(false, !initIO());
	returnIfErrE(false, !parseFile(), SMF_ERROR_DEMUX_HEADER_FAIL);
	returnIfErrC(false, !_fdura);
	returnIfErrC(false, !_fsize);
	auto fcnt = (_io->GetSize() - _hsize) / _fsize;
	returnIfErrC(false, !fcnt);
	_oport->_max = _oport->_min = _fsize;
	return init(fcnt, (uint64_t)_fdura * fcnt / 1000);
}

bool IOSourceFixedFrame::seekToIndex(uint32_t frameindex) {
	_frameIndex = frameindex;
	auto posi = _hsize + _fsize * frameindex;
	dbgTestPXL("[%u]%u", frameindex, posi);
	return _io->Seek(posi, Position::front);
}

bool IOSourceFixedFrame::generateFrame(Frame*& frm) {
	void* data = frm->LeftData();
	unsigned left = frm->Left();
	returnIf(false, left < _fsize);
	auto rsize = _io->Read(data, _fsize);
	if (!rsize && _flagsExt.Check(eLoopPlay)) {
		returnIfErrC(false, !SeekTo(0));
		returnIfErrC(false, !update(frm));
		rsize = _io->Read(data, _fsize);
	}
	returnIfNoteE(false, !rsize, SMF_ERROR_IO_EOS);
	frm->size += _fsize;
	frm->timestamp = _fdura * _frameIndex / 1000u;
	frm->index = _frameIndex++;
	return update(frm);
}
char* IOSourceFixedFrame::print(char* ptr, char* end) const {
	ptr = IOSource::print(ptr, end);
	ptr = snprintf(ptr, end, "(%u,%u,%u)"
		, _hsize, _fsize, _fdura
	);
	return ptr;
}
//IOSourceScanFrame
bool IOSourceScanFrame::open(void*) {
	returnIfErrC(false, !initIO());
	returnIfErrE(false, !parseFile(), SMF_ERROR_DEMUX_HEADER_FAIL);
	returnIfErrE(false, !scanFile(), SMF_ERROR_DEMUX_SYNC_FAIL);
	returnIfErrC(false, !_fdura);
	returnIfErrC(false, !_fcnt);
	return init(_fcnt, _fdura * _fcnt / 1000);
}

bool IOSourceScanFrame::scanFile() {
	auto posi = _hsize;
	auto fcnt = 0u;
	auto total = _io->GetSize();
	while (posi < total) {
		auto len = parseFrame(0,0);
		posi += len;
		fcnt++;
	}
	_fcnt = fcnt;
	return true;
}

bool IOSourceScanFrame::seekToIndex(uint32_t frameindex) {
	auto posi = _hsize;
	auto posi0 = posi;
	for (int i = 0; i < frameindex; i++) {
		posi0 = posi;
		returnIfErrC(false, !_io->Seek(posi, Position::front));
		auto len = parseFrame(0, 0);
		if (!len)break;
		posi += len;
	}
	returnIfErrC(false, !_io->Seek(posi0, Position::front));
	return true;
}

bool IOSourceScanFrame::generateFrame(Frame*& frm) {
	void* data = frm->LeftData();
	unsigned left = frm->Left();
	auto size = parseFrame(data,left);
	if (!size && _flagsExt.Check(eLoopPlay)) {
		returnIfErrC(false, !SeekTo(0));
		returnIfErrC(false, !update(frm));
		size = parseFrame(data, left);
	}
	returnIfNoteES(false, !size, SMF_ERROR_IO_EOS, "%d/%d", _io->GetOffset(), _io->GetSize());
	frm->size += size;
	frm->timestamp = _fdura * _frameIndex / 1000u;
	frm->index = _frameIndex++;
	return update(frm);
}

char* IOSourceScanFrame::print(char* ptr, char* end) const {
	ptr = IOSource::print(ptr, end);
	ptr = snprintf(ptr, end, "(%u,%u,%u)"
		, _hsize, _fcnt, _fdura
	);
	return ptr;
}

//IOSourceNormal
bool IOSourceNormal::parseFile() {
	returnIfErrC(false, !_fsize);
	auto media = oMedia();
	returnIfErrC(false, !media);
	auto total = _io->GetSize();
	auto fcnt = total / _fsize;
	returnIfErrC(false, !fcnt);
	uint32_t duration = 0;
	if (_fdura) {
		duration = _fdura * fcnt;
	}
	else {
		duration = media->convertByteToUs(total - _hsize);
		_fdura = duration / fcnt;
	}
	return init(fcnt,duration/1000);
}

bool IOSourceNormal::set(uint32_t key, void* val) {
	switch (key) {
	case Hash("hsize"):_hsize = (uint32_t)val; return true;
	case Hash("fsize"):_fsize = (uint32_t)val; return true;
	case Hash("fdura_us"):_fdura = (uint32_t)val; return true;
	default:break;
	}
	return IOSourceFixedFrame::set(key, val);
}
