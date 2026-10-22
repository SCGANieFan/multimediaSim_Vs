#include "smf_common.h"
#include "smf_debug.h"
//#include "app_overlay.h"

namespace smf {
	void overlay_load(const char* type, const char* keys) {}
	bool overlay_load(uint32_t id) { return true; }
	bool overlay_unload_all() { return true; }
}
EXTERNC int hal_cmu_sys_set_freq(int, int) { return 0; }