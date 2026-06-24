#include "SmfFifo.h"
#include "SmfPorting.h"
#include "smf_debug.h"
using namespace smf;

void SmfFifoBase::Initialize(fifo_t* fifo, void* data, int max) {
	if (fifo) {
		_fifo = fifo;
	}
	if (data && max) {
		_fifo->data = (uint64_t)data;
		_fifo->max = max;
		_fifo->ri = 0;
		_fifo->wi = 0;
	}
}
bool SmfFifoBase::Write(const void* src, int size, CbCopy cb, bool cache, bool is_ring) {
	returnIfErrC(false, !IsValid());
	if (!is_ring)returnIfErrC(false, Full());
	void* dst = (char*)_fifo->data + (_fifo->wi % _fifo->max) * size;
	if (cb)	
		cb(dst, src);
	else 
		memcpy(dst, src, size);
	if (cache)cache_flush(dst, size);
	_fifo->wi++;
	return true;
}
void* SmfFifoBase::Peak(uint64_t ri, int offset, int size) const {
	returnIfErrC(0, !IsValid());
	returnIf(0, Empty());
	return (char*)_fifo->data + ((ri + offset) % _fifo->max) * size;
}
bool SmfFifoBase::Read(uint64_t& ri, void* dst, int size, CbCopy cb, bool cache) {
	auto src = Peak(ri, 0, size);
	returnIf(false, !src);
	if (cache)cache_invalid(src, size);
	cb(dst, src);
	ri++;
	return true;
}
int SmfFifoBase::ReadSync(uint64_t& ri, int offset) {
	returnIfErrC(0, !_fifo);
	offset = _fifo->wi > -offset ? offset : -(int)_fifo->wi;
	ri = _fifo->wi + offset;
	return offset;
}
int SmfFifoBase::ReadSeek(uint64_t& ri, int offset) {
	returnIfErrC(0, !_fifo);
	int max = _fifo->max;
	int size = Size();
	int left = max - size;
	if(offset > size) offset = size;
	else if(offset < -left) offset = -left;
	ri = ri + offset;
	return offset;
}
int SmfFifoBase::WriteSeek(int offset, bool is_ring) {
	returnIfErrC(0, !_fifo);
	auto wi = _fifo->wi + offset;
	auto ri = _fifo->ri;
	auto max = _fifo->max;
	if (wi < ri)wi = ri;
	if (!is_ring && wi > ri + max)wi = ri + max;
	offset = wi - _fifo->wi;
	_fifo->wi = wi;
	return offset;
}
#if 0
bool testTRing() {
	TRing<smf_frame_t, 16> ring;
	uint32_t idx = 0;
	uint32_t ts = 0;
	for (int i = 0; i < 100; i++) {
		ring.Write({ nullptr,10,0,10,0u, idx,ts,nullptr }); idx++, ts += 10;
		dbgTestPDL(ring.GetIndex());
		dbgTestPDL(ring.GetMax());
		dbgTestPDL(ring.GetSize());
		dbgTestPPL(ring.GetItem());
		dbgTestPPL(ring.GetRing());
	}
	//
	auto reader = ring.CreateReader(10);
	for (int i = 0; i < 20; i++) {
		auto frm = reader.Read();
		if (frm) {
			dbgTestPDL(frm->index);
		}
		else {
			dbgTestPDL(i);
		}
	}
	//
	for (int i = 0; i < 20; i++) {
		ring.Write({ nullptr,10,0,10,0u, idx,ts,nullptr }); idx++, ts += 10;
		dbgTestPDL(ring.GetIndex());
		dbgTestPDL(ring.GetMax());
		dbgTestPDL(ring.GetSize());
		dbgTestPPL(ring.GetItem());
		dbgTestPPL(ring.GetRing());
	}
	//
	for (int i = 0; i < 20; i++) {
		auto frm = reader.Read();
		if (frm) {
			dbgTestPDL(frm->index);
		}
		else {
			dbgTestPDL(i);
		}
	}
	return true;
}

int main() {
	testTRing();
}
#endif
