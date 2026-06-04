#pragma once
#include <stdint.h>
namespace smf {	
	class mutex{
	public:
		mutex(bool init = true);
		~mutex();
	protected:
		void* _mutex = 0;
	public:
		bool initialize();
		void unlock();
		void lock();
		bool try_lock();
		void* Handle()const { return _mutex; }
		operator bool()const { return _mutex; }
	};
	class timed_mutex :public mutex {
	//public:
		//timed_mutex();
		//~timed_mutex();
	//protected:
		//void* _mutex = 0;
	public:
		//void unlock();
		//void lock();
		//bool try_lock();
		bool lock_for(uint32_t timeout_ms);
		bool lock_until(uint32_t timepoint_ms);
		//void* Handle()const { return _mutex; }
	};
	//
	template<class Mtx>
	class unique_lock {
	public:
		unique_lock() {
		}
		unique_lock(Mtx& mtx) : _mtx(&mtx) {
			mtx.lock();
		}
		unique_lock(Mtx* mtx) : _mtx(mtx) {
			if (mtx)mtx->lock();
		}
		template<class Mtxx>
		unique_lock(Mtxx& mtx,uint32_t timeout_ms) : _mtx(&mtx) {
			mtx.lock_for(timeout_ms);
		}
		~unique_lock() {
			if(_mtx)_mtx->unlock();
		}
	protected:
		Mtx* _mtx = 0;
	public:
		Mtx* mutex()const { return (Mtx*)_mtx; }
		void reset(Mtx& mtx) { reset(&mtx); }
		void reset(Mtx* mtx) {
			if (_mtx != mtx) {
				if (_mtx)
					_mtx->unlock();
				_mtx = mtx;
				if (mtx)
					mtx->lock();
			}
		}
	};	
}
