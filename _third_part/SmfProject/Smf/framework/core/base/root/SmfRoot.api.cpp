#include "SmfDef.h"
#include "SmfDef.gbl.h"
#include "SmfRoot.h"
#include "SmfPorting.h"
#include "SmfCmd.h"
#include "IPool.h"
#include "mutex.h"
#include "smf_debug.h"
namespace smf {
	static uint64_t _smf[(sizeof(SmfRoot) + 7) / 8];
	static SmfRoot& root() { return *((SmfRoot*)_smf); }
}
namespace smf {
	bool Initialize(){
		if (!root()) {
			new(&root()) SmfRoot();
		}else{
			dbgWarnPXL("smf init repeat!");
		}
		return root();
	}
	bool Deinitialize() {
		if (root()) {
			root().~SmfRoot();
		}
		return true;
	}
	//
	uint32_t UniqueID() { return root()._index++; }
	//
	Object& Root() {return root()._objs;}
	Reflection& Reflect() { return root()._reflect; }
	IPoolList& Pools() { return root()._pools; }
	IPool& BasePool() { return root()._pools.Globle(); }
	IPool* Pool() { return &root()._pools.Globle(); }
	ThreadList& Threads() { return root()._threads; }
	SmfLog& Log() { return root()._log; }

	smf_error_t& Error() { return root()._objs.Error(); }

	void* ICSBlock() { return Global(eGlobal::ics); }
	void ICSBlock(void*val) { Global(eGlobal::ics, val); }
	
	video::VideoFormatList& VideoFormats() { return *(video::VideoFormatList*)root()._vfmt; }
	audio::smf_resamples_t& AudioResamples() { return *(audio::smf_resamples_t*)root()._ares; }
	//
	mutex& Mutex() { return root()._mtx; }
	//
	uint8_t CpuID() { return root()._cpuid; }
	void CpuID(uint8_t id) { root()._cpuid = id; }
	const char* CpuName() { return (const char*)&root()._cpuname; }
	uint64_t CpuName64() { return root()._cpuname; }
	void CpuName(const char*name) { root()._cpuname = fcc64(name); }
	unsigned get_cpu_idx() { return CpuID(); }
	const char* get_cpu_name() { return CpuName(); }
	uint64_t get_cpu_name64() { return CpuName64(); }
	//
	Object& Object::Root() { return root()._objs; }
	IPoolList& IPool::Pools() { return root()._pools; }
	Reflection& ObjectCreater::Reflect() { return root()._reflect; };
	ThreadList& ThreadList::List() { return root()._threads; }
	//
	void JobHold() {
		if (!root()._jobs++) {
			Root().Set("bg/pause", false);
			Root().Set("bg/notify/pauseBg", false);
		}
	}
	void JobRelease() {
		if (!--root()._jobs) {
			Root().Set("bg/notify/pauseBg", true);
		}
	}
	uint32_t JobCount() {
		return root()._jobs.load();
	}
	//
	void*& RootGlobal(eGlobal idx) { return root()._gbls[(int)idx]; }
	void GlobalFlagsSet(eGlobalFlags mask, bool en) { root()._objs.FlagsExt().Set((uint32_t)mask, en); }
	bool GlobalFlagsGet(eGlobalFlags mask) { return root()._objs.FlagsExt().Check((uint32_t)mask); }
	void GlobalFlagsSet(EFlags mask, bool en) { root()._objs.Flags().Set((uint32_t)mask, en); }
	bool GlobalFlagsGet(EFlags mask) { return root()._objs.Flags().Check((uint32_t)mask); }
	//
	std::atomic_bool& GlobalLock(eGlobalLock idx) { return root()._locks[(int)idx]; }
}
