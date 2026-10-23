#include "SmfPortingThread.h"
#include "SmfFCC.h"
#include "smf_api.h"
#include "smf_debug.h"
#include "syslog.h"
#include "sched/sched.h"
namespace smf {
	using Item_t = ThreadItem_t;
	void* SmfGetThreadID(const char* name) {
		return 0;
	}
	bool SmfThreadUpdateInfo(Item_t* item) {
		if (item->_name == fcc32("IDLE"))
			item->_usage = 10000;
		return true;
	}

	static void log_output(const char* str, int size, int ch) {
		//hal_trace_output((const unsigned char*)str, size);
		//syslog_write(str, size);
		//if (*str == '\n') {
		//	str++;
		//	size--;
		//}
		//
		//LOG_EMERG   0
		//LOG_ALERT   1
		//LOG_CRIT    2
		//LOG_ERR     3
		//LOG_WARNING 4
		//LOG_NOTICE  5
		//LOG_INFO    6
		//LOG_DEBUG   7
		static const char lvls[]{
			LOG_ERR,	//dbg_chn_err 0
			LOG_WARNING,//dbg_chn_warn 1
			LOG_NOTICE,	//dbg_chn_cmd 2
			LOG_NOTICE,	//dbg_chn_note 3
			LOG_INFO,	//dbg_chn_info 4
			LOG_INFO,	//dbg_chn_dev 5
			LOG_INFO,	//dbg_chn_api 6
			LOG_DEBUG,	//dbg_chn_test 7
			LOG_DEBUG,	//dbg_chn_msg 8
			LOG_DEBUG,	//dbg_chn_mem 9
			LOG_ERR,	//dbg_chn_a 10//dynlibs error
			LOG_INFO,	//dbg_chn_b 11//dynlibs info
			LOG_DEBUG,	//dbg_chn_c 12//dynlibs test
			LOG_DEBUG,	//dbg_chn_media 13
			LOG_DEBUG,	//dbg_chn_script 14
			LOG_DEBUG,	//dbg_chn_io 15

			LOG_DEBUG,	//dbg_chn_msvc 0x10
			LOG_DEBUG,	//dbg_chn_ssvc 0x11
			LOG_DEBUG,	//dbg_chn_mmgr 0x12
			LOG_DEBUG,	//dbg_chn_nx 0x16
			LOG_DEBUG,	
			LOG_DEBUG,	
			LOG_DEBUG,	
			LOG_DEBUG,
			LOG_DEBUG,
			LOG_DEBUG,
			LOG_DEBUG,
			LOG_DEBUG,
			LOG_DEBUG,
			LOG_DEBUG,
			LOG_DEBUG,
			LOG_DEBUG,
			//dbg_chn_max 0x20
		};
		int lvl = lvls[ch];
		if (lvl > LOG_DEBUG)lvl = LOG_DEBUG;
		else if (lvl < LOG_ERR)lvl = LOG_ERR;

		syslog(lvl, "%.*s", size, str);
	}

	bool smf_initialize(bool reinit) {
		smf_log_redirect2(&log_output);
		return true;
	}

	int get_thread_idx() {
		struct tcb_s* tcb = this_task();
		return (int)tcb->pid;
	}

	bool os_lock(void) {
		return true;
	}
	void os_unlock(void) {
	}
}


