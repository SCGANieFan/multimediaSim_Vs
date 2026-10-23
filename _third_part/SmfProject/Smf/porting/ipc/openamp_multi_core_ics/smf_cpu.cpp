#include "smf_debug.h"
#include "smf_common.h"
#include "SmfPorting.h"
#ifdef __NuttX__
#include "nuttx/rptun/rptun.h"
#else
#include "open-amp/rpchan/rpchan.h"
#endif

extern smf_msg_eptname_cpu_t * _eptname_cpu_table;
namespace smf{
	static const char* getCpuName(uint8_t cpuid) {
		returnIfErrC(0, !_eptname_cpu_table);
		//char* cpuname = (char*)_eptname_cpu_table[cpuid].cpu_name;
		for (int i = 0; i < 8; i++) {
			auto& item = _eptname_cpu_table[i];
			if (item.cpuid && item.cpuid == cpuid) {
				return item.cpu_name;
			}
		}
		return 0;
	}
	bool cpu_open(uint8_t cpuid) {
		auto cpuname = getCpuName(cpuid);
		returnIfErrC(false, !cpuname);
		dbgDevPXL("[%u]%s", cpuid, cpuname);
#ifdef __NuttX__
		auto rst = rptun_boot(cpuname);
#else
		auto rst = rpchan_start_by_name(cpuname);
#endif
		returnIfErrCS(false, rst, "%d", rst);
		return true;
	}

	bool cpu_close(uint8_t cpuid) {
		auto cpuname = getCpuName(cpuid);
		returnIfErrC(false, !cpuname);
		dbgDevPXL("[%u]%s", cpuid, cpuname);
#ifdef __NuttX__
		auto rst = rptun_poweroff(cpuname);
#else
		auto rst = rpchan_stop_by_name(cpuname);
#endif	
		returnIfErrCS(false, rst, "%d", rst);
		return true;
	}
}
