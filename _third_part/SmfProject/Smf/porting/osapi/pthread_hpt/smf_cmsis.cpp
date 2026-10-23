#include "SmfPorting.h"
#include "smf_debug.h"
#include "pthread.h"
namespace smf {
	void* get_thread_id(void) {
		auto pid = pthread_self();
		return (void*)pid;
	}

	void sleep_for(uint32_t ms) {
		usleep(ms * 1000);
	}
	void sleep_until(uint64_t tp) {
		//osDelayUntil(ms);
		auto tp0 = get64_ms();
		if (tp > tp0)
			sleep_for(tp - tp0);
	}
	void yield() {
		// pthread_yield();
		sleep_for(1);
	}
}

