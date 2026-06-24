#include "IOPattern.h"
#include "SmfHash.h"
#include "SmfPorting.h"
#include "smf_debug.h"
#include "URL.h"

using namespace smf;
///
IOPattern::IOPattern() {
	_pattern = "%4d%02d%02d-%02d%02d%02d-%03d-%1u.log";
	_duration = 600000;
	memset(_ios, 0, sizeof(_ios));
}
IOPattern::~IOPattern() {
	Clear();
}
bool IOPattern::Clear() {
	for (auto& io : _ios) {
		delete io;
		io = 0;
	}
	return true;
}
bool IOPattern::Close(uint32_t idx) {
	auto& io = _ios[idx];
	delete io;
	io = 0;
	return true;
}

bool IOPattern::Write(uint32_t idx, void* data, uint32_t size){
	auto& io = _ios[idx];
	if (io && (get_ms() - (uint32_t)io->Tags() > _duration)) {
		delete io;
		io = 0;
	}
	if (data && size) {
		if (!io) {
			io = create();
			returnIfErrC(false, !io);
		}
		return io->Write(data, size);
	}
	return true;
}

IO* IOPattern::create() {
	char buff[256];
	auto cpuid = get_cpu_idx();
	rtc_printf(buff, buff + sizeof(buff), _pattern.c_str(), (void*)cpuid);
	URL url(buff);
	auto io = url.Create("wb");
	returnIfErrC(0, !io);
	uint32_t tp = get_ms();
	io->Tags((void*)tp);
	return io;
}