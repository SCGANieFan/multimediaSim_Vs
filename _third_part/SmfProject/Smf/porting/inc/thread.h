#pragma once
#include <stdint.h>
namespace smf {
	class thread {
	public:
		thread();
		thread(const char* name
			, void(*func)(void*para)
			, void* para			
			, unsigned stack_size
			, void* stack = 0
			, uint32_t priority  = 0
		);
		~thread();
	protected:
		const char* _name = 0;
		void* _thread_id = 0;
	public:
		bool Start(const char* name
			, void(*func)(void*)
			, void* para			
			, unsigned stack_size
			, void* stack = 0
			, uint32_t priority = 0
			);
		void* ID()const;
		const char* Name()const;
	public:
		void join();
		void detach();
	};
}
