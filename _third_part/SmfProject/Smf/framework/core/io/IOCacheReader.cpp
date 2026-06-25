#include "IOCacheReader.h"
#include "SmfFCC.h"
#include "smf_debug.h"

using namespace smf;
///
IOCacheReader::IOCacheReader() {
	_id = fcc64("iocread");
}
IOCacheReader::~IOCacheReader() {
	Close();	
}
bool IOCacheReader::Open(void*param){
	returnIfErrC(false, !IO::Open(param));
	auto para = (OpenParam*)param;
	return Reset(para->io, para->max, para->align, para->exclusive);
}
bool IOCacheReader::Close() {
	if (!IO::Close())
		return true;
	if (_io) {
		if (_exclusive) {
			_io->Close();
			delete _io;
		}
		_io = 0;
	}	
	//
	if (_buff) {
		Free(_buff);
	}
	return true;
}
bool IOCacheReader::Reset(IO* io ///io instance, can read.
	, int max  ///size of buff, align to 2^param3
	, int align ///position align
	, bool exclusive ///excluse io mode
) {
	_exclusive = exclusive;
	_io = io;
	returnIfErrC(false, !_io);
	if (_buffmax != max) {
		if (_buff)
			Free(_buff);
		_buff = (uint8_t*)Alloc(max,64);
		returnIfMemC(false, !_buff);
		_buffmax = max;
	}
	_end=_buff + max;
	_total=io->GetSize();
	_mask = align - 1;
	_buffmask = max - 1;
	//dbgTestPXL("mask:0x%08x/0x%08x",_mask, _buffmask);
	//
	_fi = _io->GetOffset();
	_ri = _bi = _bi0 = _fi;
	//dbgTestPXL("cache:%u,pos:%d/%d", max, _ri,_total);
	return true;
}
///only change read index(_ri);
bool IOCacheReader::Seek(int offset, Position pos) {
	returnIfErrC(false, !_io);
	int64_t position0 = offset;
	if (pos == Position::current) {
		position0 += _ri;
	}
	else if (pos == Position::end) {
		position0 += _total;
	}
	unsigned position = (unsigned)position0;
	returnIfErrCS(false, position0 < 0, "%u", position);
	//returnIfErrCS(false, position0 > _total,"%u>%u", position , _total);
	if (position > _total)position = _total;
	//dbgTestPXL("%08x,%08x/%08x%08x/%08x", offset, position, _total,_ri,_bi);
	_ri = position;
	return true;
}
unsigned IOCacheReader::GetSize() const {
	returnIfErrC(0, !_io);
	return _total;
}
unsigned IOCacheReader::GetOffset() const {
	returnIfErrC(0, !_io);
	return _ri;
}
unsigned IOCacheReader::Read(void* buff, unsigned size) {
	//dbgTestPXL("%08x,%08x,%08x", _ri, size, _bi);
	//auto ri0 = _ri;
	//auto fi0 = _fi;
	//
	returnIfErrC(0, !_io);
	auto rs0 = readCache(buff,size);
	if (rs0 == size) {
		return rs0;
	}
	//
	//returnIfErrC(rs0, rs0 > size);
	//	
	auto data = (uint8_t*)buff + rs0;
	auto left = size - rs0;
	auto total = rs0;
	auto end = _ri + left;
	if (end > _total){
		end = _total;
	}
	//
	auto pos0 = _ri;
	auto pos1 = (pos0 + _buffmask) &~ _buffmask;
	auto pos2 = end & ~_mask;//pos3 >> _align << _align;//align 512(FAT32 sector align)
	if (end < pos1) {
		auto rsize = left;
		returnIfIOC(total,!syncFill());
		auto rs = readCache(data, rsize);
		total += rs;
		/*if (rs < rsize) {
			rs = rs;
		}*/
		returnIfIOCS(total, rs < rsize, "%d<%d,%d~%d/%d", rs, rsize, _fi, _ri, _total)
		return total;
	}
	if (pos1 > pos0) {//read dirent unaligned
		auto rsize = pos1 - pos0;
		auto rs = readDirect(data, rsize);
		returnIfIOC(total, !rs);
		total += rs;
		data += rs;
		returnIfIOCS(total, rs < rsize, "%d<%d,%d~%d/%d", rs, rsize, _fi, _ri, _total);
	}
	if (pos2 > pos1) {//read dirent aligned
		auto rsize = pos2 - pos1;
		auto rs = readDirect(data, rsize);
		returnIfIOC(total, !rs);
		total += rs;
		data += rs;
		/*if (rs < rsize) {
			rs = rs;
		}*/
		returnIfIOCS(total, rs < rsize, "%d<%d,%d~%d/%d", rs, rsize, _fi, _ri, _total)
	}
	if (end > pos2) {
		auto rsize = end - pos2;
		returnIfIOC(total, !syncFill());
		auto rs = readCache(data, rsize);
		total += rs;
		data += rs;
		/*if (rs < rsize) {
			rs = rs;
		}*/
		returnIfIOCS(total, rs < rsize, "%d<%d,%d~%d/%d", rs, rsize, _fi, _ri, _total);
	}
	/*if (total != size) {
		dbgErrPXL("%d,%d", total, size);
	}*/
	returnIfIOCS(total, total != size, "%d<%d,%d~%d/%d", total, size, _fi, _ri, _total);
	return total;
}
bool IOCacheReader::syncFill() {
	returnIfErrC(false, !syncIO());
	auto bi = _fi & _buffmask;
	auto data = _buff + bi;
	auto size = _buffmax - bi;
	_bi0 = _fi;
	auto rs = readIO(data, size);
	returnIfWarnCS(false,!rs,"%p,%d,%u/%u",data,size,GetOffset(),GetSize());
	_bi = _fi;
	return true;
}
unsigned IOCacheReader::readIO(void* data,unsigned size) {
	returnIfIOC(0, !size);
	auto rs = _io->Read(data, size);
	returnIfIOCS(0, !rs, "current size: %d", GetOffset());
	_fi += rs;
	return rs;
}
bool IOCacheReader::syncIO() {
	if (_fi != _ri) {///sync file inded(_fi)
		returnIfErrCS(false, !_io->Seek(_ri, Position::front), "%u>%u", _ri, (unsigned)GetSize());
		_fi = _ri;
	}
	return true;
}
unsigned IOCacheReader::readDirect(void* data, unsigned size) {
	returnIfErrC(false, !syncIO());
	auto rs = readIO(data, size);
	_ri = _fi;
	return rs;
}
unsigned IOCacheReader::readDirect(void* data) {
	auto bi = _fi & _buffmask;
	auto size = _buffmax - bi;
	return readDirect(data,size);
}
unsigned IOCacheReader::readCache(void* buff, unsigned size) {
	if (_ri < _bi0) {		
		return 0;
	}
	if (_ri >= _bi) {
		return 0;
	}
	//
	auto left = _bi - _ri;
	if (size > left) {
		size = left;
	}
	int off = _ri & _buffmask;
	memcpy(buff, _buff + off, size);
	_ri += size;
	return size;
}
unsigned IOCacheReader::Write(void* buff, unsigned size) {
	return 0;
}

int IOCacheReader::Skip(int size) {
	returnIfErrC(0, !_io);
	if (_ri < _bi0) {
		return 0;
	}
	if (_ri >= _bi) {
		return 0;
	}
	int left = _bi - _ri;
	if (size > left) {
		size = left;
	}
	_ri += size;
	return size;
}

uint8_t* IOCacheReader::Peek(int& size) const{
	returnIfErrC(0, !_io);
	if (_ri < _bi0) {
		return 0;
	}
	if (_ri >= _bi) {
		return 0;
	}
	size = _bi - _ri;
	auto ri = _ri & _buffmask;
	return _buff + ri;
}
int IOCacheReader::Peek(void* output, int size) const {
	int size0 = 0;
	void* data = Peek(size0);
	if (!data)return 0;
	if (size > size0)size = size0;
	memcpy(output, data, size);
	return size;
}
bool IOCacheReader::SyncFill() {
	returnIfErrC(false, !_io);
	return syncFill();
}
