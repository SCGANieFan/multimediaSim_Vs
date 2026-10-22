#include "smf_common.h"
#include "smf_debug.h"
//#include "app_overlay.h"

EXTERNC void app_overlay_unloadall();
EXTERNC void app_overlay_select(uint32_t id);
namespace smf {
	bool overlay_load(uint32_t id) {
		dbgTestPDL(id);
		app_overlay_unloadall();
		app_overlay_select(id);
		return true;
	}
	bool overlay_unload_all(){
		app_overlay_unloadall();
		return true;
	}
}
