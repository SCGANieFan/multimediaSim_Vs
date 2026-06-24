#include "SmfDef.h"
#include "SmfRingBuffer.h"
#include "smf_api.h"
#include "smf_debug.h"
#include "SmfPorting.h"
#include "SmfICS.h"
using namespace smf;

typedef void (*cbFunc)(const char* str, int size);
static cbFunc _cb_func = 0;

static void cbWrite(const char* str, int size) {
	auto fifo = (smf_fifo_t*)ics::GetLogFifo();

	//auto cpuid = get_cpu_idx();
	//auto shm = ICSBlock();
	//if (shm && cpuid) {
	//	auto& fifo = shm->cpus[cpuid].log;
		if (fifo->data) {
			SmfRingBuffer ring;
			ring.Initialize(fifo);
			ring.Write(str, size, true, true);
		}
		if (_cb_func) {
			_cb_func(str, size);
		}
	//}
}

EXTERNC void smf_log_redirect_to_background(void* buff, int size) {
	dbgTestPL();
	//auto cpuid = get_cpu_idx();
	auto shm = ICSBlock();
	//auto& fifo = shm->cpus[cpuid].log;
	auto fifo = (smf_fifo_t*)ics::GetLogFifo();
	dbgTestPPL(fifo);
	auto data = ((uint32_t)buff + 63) & ~63;
	auto data_end = ((uint32_t)buff + size) & ~63;
	size = data_end - data;
	if (fifo && !fifo->data) {
		fifo->data = (uint64_t)data;
		fifo->max = size;
		fifo->ri = 0;
		fifo->wi = 0;
		dbgTestPXL("%p,%p/%p", shm, fifo->data, fifo->max);
	}
	smf_log_redirect1(&cbWrite);
}

EXTERNC void smf_log_redirect_to_background1(int size) {
	auto buff = Alloc(size,64);
	returnIfErrC0(!buff);
	smf_log_redirect_to_background(buff, size);
}

EXTERNC void smf_shm_log_register_cb(void (*cb)(const char* str, int size)) {
	_cb_func = cb;
}
