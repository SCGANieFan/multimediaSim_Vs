#pragma once
#include <stdint.h>
namespace smf {
	class semaphore{
	public:
		semaphore(int cnt = 0, bool init = true);
		~semaphore();
	protected:		
		void* _sema = 0;
	public:
		bool initialize(int cnt = 0);
		uint32_t count()const;
		bool reset(int cnt);
		bool post();
		bool wait();
		bool try_wait();
		bool wait_for(uint32_t timeout);
		bool wait_until(uint32_t timepoint);
		void* sema(){return _sema;}
		bool valid() const{return _sema;}
		operator bool()const { return _sema; }
	};
}
