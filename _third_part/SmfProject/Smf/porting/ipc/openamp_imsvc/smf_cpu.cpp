#include "smf_debug.h"
#include "smf_common.h"
#include "SmfPorting.h"
#include "IMsvc.h"
#ifdef __NuttX__
#include "nuttx/rptun/rptun.h"
#else
#include "open-amp/rpchan/rpchan.h"
#endif

namespace smf{
	static const char* getCpuName(uint8_t cpuid) {
		auto svc = api::IMsvc::Msvc();
		returnIfErrC(0, !svc);
		//char* cpuname = (char*)_eptname_cpu_table[cpuid].cpu_name;
		return svc->GetIpc(cpuid);
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
