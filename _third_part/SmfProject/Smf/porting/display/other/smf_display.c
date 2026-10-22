#include "smf_common.h"
#include "smf_api_display.h"
#include "smf_debug.h"

WEAK void* smf_display_open(void* para) {
    dbgErrPL();
    return 0;
}
WEAK bool smf_display_config(void* handle, smf_display_config_t* cfg) {
    dbgErrPL();
    return false;
}
WEAK bool smf_display_trigger(void* handle, void* fbaddr, uint32_t fdsize) {
    dbgErrPL();
    return false;
}
WEAK bool smf_display_close(void* handle) {
    dbgErrPL();
    return false;
}
