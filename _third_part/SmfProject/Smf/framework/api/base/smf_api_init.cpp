#include "SmfDef.h"
#include "SmfPorting.h"
#include "SmfLayout.h"
#include "JsonLayout.h"
#include "smf_api.h"
#include "SmfICS.h"
#include "SmfFCC.h"
#include "unique_ptr.h"
#include "URL.h"
using namespace smf;
//
EXTERNC bool smf_init() { 
	return Initialize();
}
EXTERNC void smf_uninit() { 
	Deinitialize(); 
}
EXTERNC void smf_set_cpuid(smf_cpu_id_e cpuid) {
	CpuID(cpuid);
}
EXTERNC void smf_set_cpu(smf_cpu_id_e cpuid, const char* name) {
	CpuID(cpuid);
	CpuName(name);
}
EXTERNC uint8_t smf_get_cpuid() {
	return CpuID();
}

EXTERNC bool smf_config_from_file(const char* url) {
	VoidBuffer buff;
	returnIfErrC(false,!URL::Read(url, buff));
	auto ext = strrchr(url, '.');
	switch (Hash(ext)) {
	case Hash(".json"):return smf_config(buff.get(), true, false, false);
	case Hash(".cpp"):return smf_config(buff.get(), false, true, false);
	case Hash(".cfg"):return smf_config(buff.get(), false, false, false);
	case Hash(".bin"):return smf_config(buff.get(), true, false, true);
	default:return false;
	}
}

EXTERNC bool smf_config(void* script, bool json, bool cpp, bool bin) {
	if (bin) {
		uint32_t* offsets = (uint32_t*)script;
		auto cpuid = get_cpu_idx();
		auto offset = offsets[cpuid];
		returnIfErrC(false, !offset);
		script = (char*)script + offset;
		return smf_config(script, json, cpp, false);
	}
	else if (json) {
		return JsonLayout().Layout((const char*)script);
	}
	else if (cpp) {
		return Layout((const char*)script, false, false, true).Create();
	}
	else {
		return Layout((const char*)script, false, false, false).Create();
	}
}

static bool ParseJson(const char* script, const char*& begin, const char*& end) {
	auto ptr = script;
	while (*ptr) {
		if (ptr[0] == '@' && 
			ptr[1] == 'v' && 
			ptr[2] == 'e' && 
			ptr[3] == 'r' && 
			ptr[4] == 's' && 
			ptr[5] == 'i' && 
			ptr[6] == 'o' && 
			ptr[7] == 'n' && 
			ptr[8] == '@'
			) {
			ptr += 8;
			Root().Set("script_version", ptr);
			break;
		}
		ptr++;
	}
	auto cpu = CpuName64();
	auto find = false;
	while (*ptr) {
		if (ptr[0] == '@' && 
			ptr[1] == 's' && 
			ptr[2] == 'm' && 
			ptr[3] == 'f' &&
			ptr[4] == '@'
			) {
			auto name = fcc64(ptr + 5);
			//dbgTestPXL("%u,%s,%s", find, &cpu, &name);
			if (name == cpu) {
				while (*ptr && *ptr != '{')ptr++;
				returnIfErrC(0, *ptr != '{');
				begin = ptr;
				find = true;
			}
			else if (find) {
				ptr -= 1;
				while (ptr > script && *ptr != ',')ptr--;
				returnIfErrC(0, *ptr != ',');
				end = ptr;
				return true;
			}
			else {
				ptr += 5;
			}
		}
		ptr++;
	}
	return false;
}
#define ALIGN64(x) (((x) + 63) & ~63)
EXTERNC bool smf_config_from_ics() {
	auto script = ics::GetScript(true);
	returnIfErrC(false, !script);
	const char* begin = 0;
	const char* end = 0;
	returnIfErrC(false, !ParseJson(script, begin, end));
	//dbgTestPSL(script);
	//dbgTestPXL("%p,%p,%p", script, begin, end);
	returnIfErrC(false, !begin);
	returnIfErrC(false, !end);
	//dbgTestPXL(begin);
	//dbgTestPXL(end);
	auto len = end - begin;
	auto size = len + 1 + 8;
	auto buff = (char*)Alloc(ALIGN64(size), 64);
	memcpy(buff, begin, size);
	buff[len] = 0;
	Root().Set("script", buff);
	//dbgInfoPXL("script memory:%p,%u", buff, size);
	returnIfErrC(false, !JsonLayout().Layout((const char*)buff, len, 0, true));
	cache_writeback(buff, ALIGN64(size));
	return true;
}

EXTERNC bool smf_config_from_json(const char* script) {
	returnIfErrC(false, !script);
	const char* begin = 0;
	const char* end = 0;
	returnIfErrC(false, !ParseJson(script, begin, end));
	//dbgTestPSL(script);
	//dbgTestPXL("%p,%p,%p", script, begin, end);
	returnIfErrC(false, !begin);
	returnIfErrC(false, !end);
	//dbgTestPXL(begin);
	//dbgTestPXL(end);
	auto len = end - begin;
	auto size = len + 1 + 8;
	auto buff = (char*)Alloc(size, 64);
	memcpy(buff, begin, size);
	buff[len] = 0;
	Root().Set("script", buff);
	dbgInfoPXL("script memory:%p,%u", buff, size);
	returnIfErrC(false, !JsonLayout().Layout(buff, len, 0, true));
	cache_writeback(buff, size);
	return true;
}
