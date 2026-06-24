#include "SmfDef.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "SmfICS.h"
#include "SmfLog.shm.service.h"
#include "SmfFifoBuffer.h"
#include "SmfPorting.h"
#include "URL.h"
#include "smf_debug.h"

using namespace smf;
EXTERNC void smf_log_service_register() {
	ShmLogService::Register<ShmLogService>("slog");
}

ShmLogService::ShmLogService() {
	_id = fcc64("slog");
}
ShmLogService::~ShmLogService() {
	if (_io)delete _io;
}
bool ShmLogService::set(uint32_t keys, void* vals) {
	switch (keys) {
	case Hash("file"):
	case Hash("url"): _url = (const char*)vals; return true;
	case Hash("duration"): _duration = (uint32_t)vals; return true;
	}
	return Base::set(keys, vals);
}

bool ShmLogService::updateIO() {
	auto tp = get_local_ms();
	if (_io) {
		if (_duration && _tp && _tp + _duration <= tp) {
			delete _io;
			_io = 0;
		}
		else if (!_url) {
			delete _io;
			_io = 0;
		}
	}
	if (!_io) {
		if (_url) {
			_io = URL(_url.c_str()).Create("wb");
			returnIfErrC(false, !_io);
			_tp = tp;
		}
	}
	return true;
}

bool ShmLogService::run(void* priv) {
	auto shm = ICSBlock();
	returnIfErrC(false, !shm);
	for (int i = 1; i < 8; i++) {
		auto cpuid = i;
		auto log = (smf_fifo_t*)ics::GetLogFifo(cpuid);
		// dbgTestPXL("fifo=%p,cpuid = %d", log, cpuid);
		// dbgTestPXL("%u, %p, %p %d, %p, ,%d,%d/%d",cpuid, shm, &cpu, cpu.logEn,cpu.log.data,cpu.log.max, (uint32_t)cpu.log.ri, (uint32_t)cpu.log.wi);
		if (ics::IsPowerOn(i) && log->data && log->max && log->ri != log->wi) {
			SmfFifoBuffer fifo(log);
			if (int loss = fifo.ReadCheck(true)) {
				dbgWarnPXL("LOST[%u]%u", cpuid, loss);
			}
			void* data = 0;
			auto size = fifo.ReadPointer(data, true);
			if (data && size) {
				if (_io) {
					_io->Write(data, size);
				}
				else {
					dbgOutput(0, data, size);
				}
				fifo.ReadSeek(size);
			}
		}
	}
	updateIO();
	return true;
}

