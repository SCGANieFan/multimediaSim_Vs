#include "SmfFifoBuffer.h"
#include "smf_debug.h"
#include "SmfPorting.h"
using namespace smf;

SmfFifoBuffer::SmfFifoBuffer(fifo_t* fifo, void* data, uint32_t max) {
	Initialize(fifo, data, max);
}
void SmfFifoBuffer::Initialize(fifo_t* fifo, void* data, uint32_t max) {
	returnIfErrC0(!fifo);
	//dbgTestPXL("%p,%p,%d",fifo, data, max);
	if (fifo) {
		_fifo = fifo;
	}
	if (_fifo && data && max) {
		// dbgTestPXL("%p,%p,%d", fifo, data, max);
		_fifo->data = (uint64_t)data;
		_fifo->max = max;
		_fifo->wi = 0;
		_fifo->ri = 0;
	}
}
void SmfFifoBuffer::Uninit() {
	_fifo = 0;
}
#if 0
bool SmfFifoBuffer::IsValid()const {return _fifo && _fifo->data;}
uint64_t SmfFifoBuffer::WriteIndex()const {return _fifo->wi;}
uint64_t SmfFifoBuffer::ReadIndex()const {return _fifo->ri;}
uint32_t SmfFifoBuffer::Max()const {return _fifo->max;}
uint32_t SmfFifoBuffer::Size()const {return (uint32_t)(_fifo->wi - _fifo->ri);}
uint32_t SmfFifoBuffer::Left()const {return Max() - Size();}
SmfFifoBuffer::fifo_t* SmfFifoBuffer::Fifo()const {return (fifo_t*)_fifo;}
#endif
uint32_t SmfFifoBuffer::Fill(char ch, uint32_t count, bool cache, bool is_ring){
	void* data = 0;
	auto size = WritePointer(data, is_ring);
	if (!size) return 0;
	if (size >= count) {
		memset(data, ch, count);
		if (cache) {
			//cache_writeback(data, count);
			auto max = _fifo->max;
			auto buff = (char*)_fifo->data;
			cache_flush(buff, max);
		}
		WriteSeek(count, is_ring);
		return count;
	}
	else {
		memset(data, ch, size);
		WriteSeek(size);
		return size+ Fill(ch, count-size);
	}
}
uint32_t SmfFifoBuffer::Write(const void* data, uint32_t size, bool all, bool cache, bool is_ring, bool monoblock, void** target) {
	returnIfErrC(false, !_fifo);
	auto wi = _fifo->wi;
	auto max = _fifo->max;
	auto buff = (char*)_fifo->data;
	auto left = Left();
	auto wii = wi % max;
	returnIfInfoC(0, !data);
	returnIfInfoC(0, !size);
	returnIfInfoC(0, !max);
	//returnIfInfoC(0, size > max);
	if (size > max) {
		if (all || monoblock)return 0;
		size = max;
	}
	if (!is_ring) {//check reader position
		if (size > left) {
			if (all)return 0;
			size = left;
		}
	}
	if (monoblock) {//write traget is whole continuous block.
		auto lsize = max - wii;
		if (size > lsize) {
			if (!is_ring && all && (size + lsize > left))return 0;
			wii = 0;
			wi += lsize;
			left -= lsize;
		}
	}
	//
	if (target)*target = buff + wii;
	if (wii + size <= max) {
		memcpy(buff + wii, data, size);
	}
	else {
		uint32_t size1 = wii + size - max;
		uint32_t size0 = size - size1;
		memcpy(buff + wii, data, size0);
		memcpy(buff, (char*)data + size0, size1);
	}
	if (cache) {
		cache_flush(buff, max);
	}
	//
	_fifo->wi = wi + size;
	//dbgTestPXL("%u/%u,%d,%d", _fifo->ri,_fifo->wi, size, _fifo->max);
	return size;
}

uint32_t SmfFifoBuffer::WritePointer(void*& data, bool is_ring) const {
	returnIfErrC(0, !_fifo);
	data = 0;
	auto wi = _fifo->wi;
	auto max = _fifo->max;
	auto buff = (char*)_fifo->data;
	returnIfInfoC(0, !max);
	auto size = is_ring ? max : Left();
	auto wii = wi % max;	
	data = buff + wii;
	if (wii + size > max ) {
		size = max - wii;
	}
	return size;
}
int SmfFifoBuffer::WriteSeek(int offset, bool is_ring) {
	returnIfErrC(0, !_fifo);
	auto ri = _fifo->ri;
	auto wi = _fifo->wi;
	int max = _fifo->max;
	//
	if (!is_ring) {
		if (offset > 0 && wi + offset > ri + max) {
			offset = ri + max - wi;
		}
		else if (offset < 0 && wi + offset < ri) {
			offset = (int64_t)ri - wi;
		}
	}
	//
	_fifo->wi = wi + offset;
	return offset;
}
///
uint32_t SmfFifoBuffer::Read(uint64_t& ri, void* data, uint32_t size, bool all, bool cache) {
	returnIfErrC(0, !_fifo);
	uint32_t rsize = Peak(ri, data, size, all, cache);
	ri += rsize;
	//dbgTestPXL("[%p]%u=>%u,%u=>%u,%u",this,_fifo->wi,_ri,size,rsize,GetLeft());
	return rsize;
}
uint32_t SmfFifoBuffer::Peak(uint64_t& ri, void* data, uint32_t size, bool all, bool cache)const{
	returnIfErrC(0, !_fifo);
	returnIfErrC(0, !data);
	returnIfErrC(0, !size);
	auto wi = _fifo->wi;
	uint32_t left = wi - ri;
	auto max = _fifo->max;
	auto buff = (char*)_fifo->data;
	if (left > max) {
		dbgErrPXL("%u+%u<%u", (uint32_t)ri, max, (uint32_t)wi);
	}
	if (size > left) {
		returnIf(0, all);
		size = left;
	}
	//
	if (cache) {
		size &= ~(64 - 1);
	}
	if (!size) {
		return 0;
	}
	//auto ri = _fifo->ri;
	
	returnIfErrC(0, size > max);
	returnIfErrC(0, !buff);	
	auto rii = ri % max;
	if (cache) {
		cache_invalid(buff, max);
	}
	if (rii + size <= max) {
		//if (cache) {
		//	cache_invalid(buff + rii, size);
		//}
		memcpy(data, buff + rii, size);
	}
	else {
		auto size0 = max - rii;
		//if (cache) {
		//	cache_invalid(buff + rii, size0);
		//	cache_invalid(buff, size - size0);
		//}
		memcpy(data, buff + rii, size0);
		memcpy((char*)data + size0, buff, size - size0);
	}
	return size;
}
uint32_t SmfFifoBuffer::ReadPointer(uint64_t& ri, void*& data, bool cache)const{
	returnIfErrC(0, !_fifo);
	auto max = _fifo->max;
	//returnIfErrC(0, !max);
	auto buff = (char*)_fifo->data;
	//returnIfErrC(0, !buff);		
	auto size = Size(ri);
	returnIf(0, !size);
	//auto wi = _fifo->wi;
	//auto ri = _fifo->ri;
	auto rii = ri % max;
	if (rii + size > max) {
		size = max - rii;
	}	
	data = buff + rii;
	if(cache) cache_invalid(buff, max);
	return size;
}

int SmfFifoBuffer::ReadSeek(uint64_t& ri, int offset, bool check) const {
	returnIfErrC(0, !_fifo);
	//auto wi = _fifo->wi;
	//auto ri = _fifo->ri;
	//auto mi = _fifo->mi;
	int max = _fifo->max;
	int size = Size(ri);
	int left = max - size;
	if (check) {
		if (offset > size)offset = size;
		else if (offset < -left) offset = -left;
	}
	//
	ri = ri + offset;
	return offset;
}
int SmfFifoBuffer::ReadSync(uint64_t& ri, int offset, bool check) const {
	auto wi = _fifo->wi;
	int mi = _fifo->max;
	if (check) {
		if (offset > mi)
			offset = mi;
		else if (offset < -mi)
			offset = -mi;
	}
	auto ri0 = ri;
	ri = wi + offset;
	return ri - ri0;
}
int SmfFifoBuffer::ReadCheck(uint64_t& ri_, bool sync) const {
	auto wi = _fifo->wi;
	auto mi = _fifo->max;
	auto ri = ri_;
	if (ri > wi)
		ri = wi;
	else if (ri + mi < wi)
		ri = wi - mi;
	int offset = ri - ri_;
	if (sync)ri_ = ri;
	return offset;
}

void* SmfFifoBuffer::memalloc(uint32_t size) {
	auto len = (size + 7) & ~7;
	auto left = Size();
	//dbgTestPXL("%u,%u>%u", size, len, left);
	returnIf(0, len > left);
	//returnIfWarnCS(0, len > left, "%u,%u>%u", size,len,left);
	void* rdata = 0;
	auto rsize = ReadPointer(rdata, false);
	//dbgTestPXL("%p,%u", rdata, rsize);
	returnIfWarnC(0, !rsize);
	returnIfWarnC(0, !rdata);
	if (len > rsize) {
		returnIf(0, len > left - rsize);
		//returnIfWarnC(0, len > left - rsize);
		ReadSeek(rsize);
		//dbgTestPXL("%u,%u>%u-%u", size, len, left, rsize);
		return memalloc(size);
	}
	else {
		ReadSeek(len);
		return rdata;
	}
}
bool SmfFifoBuffer::memfree(void* ptr, uint32_t size) {
	auto len = (size + 7) & ~7;
	//dbgTestPXL("%p,%u,%u", ptr, size, len);
	void* wdata = 0;
	auto wsize = WritePointer(wdata, false);
	returnIfErrC(false, !wdata);
	returnIfErrC(false, !wsize);
	if (len > wsize) {
		len += wsize;
		wdata = (void*)_fifo->data;
	}
	if (wdata != ptr) {
		dbgErrPXL("%u,%u,%p,%p", size, wsize, ptr, wdata);
	}
	return WriteSeek(len);
}

SmfFifoBuffer::Reader SmfFifoBuffer::CreateReader()const {
	return Reader{ _fifo,_fifo->wi };
}

SmfFifoBuffer::Reader SmfFifoBuffer::CreateReader(uint64_t ri)const {
	return Reader{_fifo, ri };
}

void FifoBufferReader::CheckAndAjustRi(int align) {
	auto ri = _ri;
	auto wi = WriteIndex();
	auto size = Size();
	auto max = Max();
	if (size > max) {
		// left %= max;
		ri = wi - size % max;
		if (align) {
			auto mask = align -1;
			ri = (ri + mask) & ~mask;
		}
		dbgWarnPXL("sync ri:%u,%u/%u,%u,%u", max, (uint32_t)_ri, (uint32_t)ri, (uint32_t)wi, align);
		_ri = ri;
	}
}
///
SmfFifoBufferD::~SmfFifoBufferD() {
	Uninit();
}
SmfFifoBufferD::SmfFifoBufferD(int max, int align) {
	if (max)
		Initialize(max, align);
	else
		memset(&_fifoD, 0, sizeof(_fifoD));
}
bool SmfFifoBufferD::Initialize(uint32_t max, int align) {
	auto data = Alloc(max, align); //dbgTestPPL(data);
	returnIfErrC(false, !data);
	SmfFifoBuffer::Initialize(&_fifoD, data, max);
	return true;
}
void SmfFifoBufferD::Uninit() {
	Free((void*&)_fifoD.data);
	_fifoD.data = 0;
}

