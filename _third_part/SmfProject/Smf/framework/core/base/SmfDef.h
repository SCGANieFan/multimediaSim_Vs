#pragma once
#include "smf_common.h"
#include "smf_error.h"

struct smf_ics_t;

namespace smf {
	using timepoint64_t = smf_timepoint64_t;
	using duration64_t = smf_duration64_t;
	using timepoint32_t = smf_timepoint32_t;
	using duration32_t = smf_duration32_t;
	using percentage_t = smf_percentage_t;
	using frameindex_t = smf_frameindex_t;
	using region_t = smf_region_t;
	using fraction_t = smf_fraction_t;

	class Object;
	class Reflection;
	class IPoolList;
	class IPool;
	class ThreadList;
	class SmfLog;
	class BackGround;
	class mutex;
	namespace video {
		class VideoFormatList;
	}
	namespace audio {
		struct smf_resamples_t;
	}
	//root
	Object& Root();
	Reflection& Reflect();
	IPoolList& Pools();
	IPool& BasePool();
	IPool* Pool();
	ThreadList& Threads();
	SmfLog& Log();
	smf_error_t& Error();
	void* ICSBlock();
	void ICSBlock(void*);
	video::VideoFormatList& VideoFormats();
	audio::smf_resamples_t& AudioResamples();
	mutex& Mutex();
	uint8_t CpuID();
	void CpuID(uint8_t);
	const char* CpuName();
	uint64_t CpuName64();
	void CpuName(const char*);
	//
	enum class Result {
		True = 1,
		False = 0,
		Other = -1,
	};

	template<class K, class V>
	class Pair {
	public:
		K keys;
		V vals;
	};

	template<class T1, class T2>
	class Tuple2 {
	public:
		T1 _1;
		T2 _2;
	};

	template<class T1, class T2, class T3>
	class Tuple3 {
	public:
		T1 _1;
		T2 _2;
		T3 _3;
	};

	template<class T1, class T2, class T3, class T4>
	class Tuple4 {
	public:
		T1 _1;
		T2 _2;
		T3 _3;
		T4 _4;
	};

	//
	bool Initialize();
	bool Deinitialize();
	//
	int snprintf(char*, int, const char*, ...);
	char* snprintf(char*, const char*, const char*, ...);
	uint64_t UniqueID(const char* pre);
	uint64_t UniqueID(char pre);
	uint32_t UniqueID();
	uint64_t Combine64(const char* pre, uint32_t idx);
	uint64_t Combine64(char pre, uint32_t idx);
	//
	bool Configure(const char* url, bool is_cpp = false);
	Object* Deserialize(const char* serial, void* params = 0, Object* parent = 0);
	Object* Deserialize(const char* serial, void* params, const char* parent);
	Object* Deserialize(mutex& mtx, const char* serial, void* params = 0, Object* parent = 0);
	Object* Deserialize(mutex& mtx, const char* serial, void* params, const char* parent);
	bool Deserialize(uint8_t cpuid, const char* serial, void* params = 0, uint32_t parasize = 0);

	typedef bool(*CbInvoke)(void*);
	bool Invoke(bool(*func)(void*),void* priv = 0, uint32_t delay = 0, uint32_t interval = 0);
	bool InvokeFree(void* buff, uint32_t delay = 0);
	bool InvokeDelete(Object* obj, uint32_t delay = 0);

	void* Alloc(unsigned size, unsigned align = 0);
	void* Calloc(unsigned count, unsigned size);
	void* Realloc(void* ptr, unsigned size);
	bool Hold(void* ptr);
	void Free(void*& ptr);
	
	template<class T>inline
	T* Alloc(unsigned size, unsigned align = 0) { return (T*)Alloc(size, align); }
	template<class T> static inline
	T* Calloc(unsigned num, unsigned size) { return (T*)Calloc(num, size); }
	template<class T> static inline
	T* Realloc(void* ptr, unsigned size) { return (T*)Realloc(ptr, size); }
	template<class T>inline
	void Free(T*& ptr) { Free((void*&)ptr); }
	//
	void JobHold();
	void JobRelease();
	uint32_t JobCount();
	//
	typedef void (*CbEntry)(int argc, char** argv);
	bool Register(const char*name, CbEntry callback);
	int Entry(int argc, char** argv);
	//
	bool RemoteSet(uint8_t cpuid, const char* path, const void* params, int size);
	bool RemoteSet(uint8_t cpuid, const char* path, int vals);
	bool RemoteConfig(uint8_t cpuid, const char* script, const void* params, int size);
	//
	bool MsvcInvoke(smf_cb_priv0 cb, void* priv);
	bool MsvcInvokeFree(void* buff);
	bool MsvcInvokeDelete(Object* obj);
	//
	bool PowerOn(uint8_t cpuid);
	bool PowerOff(uint8_t cpuid);
	bool SysFreqRequest(uint32_t freq = 0xffffffff);
	//
	void ErrorProcess(smf_error_t& err, const char* file, unsigned line, unsigned error);
}
