#pragma once
#include "Object.def.h"
#include "Locker.h"
namespace smf {
	enum class eGlobal {
		ics = 0,
		cmd,
		background,
		msgcore,
		msvc,
		sclient,
		ssvc,
		mmgr,
		bgnotify,
		max
	};
	void*& RootGlobal(eGlobal idx);
	template<class T = void>
	T* Global(eGlobal idx) {
		return (T*)RootGlobal(idx);
	}
	template<class T = void>
	void Global(eGlobal idx, T* val) {
		RootGlobal(idx) = val;
	}

	enum class eGlobalFlags {
		measureLock = 1u << 0,
		powerCtrl_1 = 1u << 1,
		powerCtrl_2 = 1u << 2,
		powerCtrl_3 = 1u << 3,
		powerCtrl_4 = 1u << 4,
		powerCtrl_5 = 1u << 5,
		powerCtrl_6 = 1u << 6,
		powerCtrl_7 = 1u << 7,
	};

	void GlobalFlagsSet(eGlobalFlags mask, bool en);
	bool GlobalFlagsGet(eGlobalFlags mask);

	void GlobalFlagsSet(EFlags mask, bool en);
	bool GlobalFlagsGet(EFlags mask);

	enum class eGlobalLock {
		lock0 = 0,
		lock1,
		lock2,
		lock3,
		lock4,
		lock5,
		lock6,
		lock7,
		max,
	};
	std::atomic_bool& GlobalLock(eGlobalLock idx);
	class GlobalLocker: public AtomicLocker {
	public:
		GlobalLocker(eGlobalLock idx) : AtomicLocker(GlobalLock(idx)) { Acquire(); }
		~GlobalLocker() { Release(); }
	};
}

