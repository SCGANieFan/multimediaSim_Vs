#pragma once
#include "Object.h"
#include "SmfDef.gbl.h"
#include "IPoolList.h"
#include "Reflection.h"
#include "SmfLog.h"
#include "SmfFCC.h"
#include "ThreadList.h"
#include "mutex"
#include <atomic>
namespace smf {
	class SmfRoot{
	public:
		SmfRoot();
		~SmfRoot();
	public:
		template<int C>
		class TRoot : public Object::List<C> {
		public:
			TRoot() { Object::_id = fcc32("root"); }
		protected:
			using Base = Object::List<C>;
			virtual bool get(uint32_t, void*)const override;
			virtual bool set(uint32_t, void*)override;
			virtual char* print(char* ptr, char* end) const override;
		public:
			virtual Object::ParamTable* GetParamTable() const override;
		public:
			SmfRoot& root()const { return *(SmfRoot*)this->_tags; }
		};
	public:
		uint64_t _begin = fcc64("smf0smf1");
		uint32_t _size = sizeof(SmfRoot);
		//
		mutex _mtx;
		std::atomic<uint32_t> _index = 0;
		TSmfLog<1024> _log;
		TPoolList<8> _pools;
		TReflection<128> _reflect;
		TRoot<16> _objs;
		TThreadList<16> _threads;
		void* _vfmt[32 * 2 + 1 + 4];
		void* _ares[4];
		uint8_t _cpuid = 0;
		uint32_t _rev : 24;
		uint64_t _cpuname = 0;
		//
		std::atomic<int32_t> _jobs = 0;
		const char* _script = 0;
		const char* _script_version = 0;
		const char* _version = 0;
		void* _gbls[(int)eGlobal::max];
		std::atomic_bool _locks[(int)eGlobalLock::max];
		uint32_t _esize = sizeof(SmfRoot);
		uint64_t _end = fcc64("smf2smf3");
	public:
		operator bool() { return (_begin == fcc64("smf0smf1"))&& (_end == fcc64("smf2smf3")); }
	public:
		static void* operator new(size_t size, void* ptr) { return ptr; }
	};
}
