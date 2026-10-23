#include "SmfPorting.h"
#include "smf_debug.h"
//#include "hal_intersys.h"
EXTERNC int hal_sco_intersys_register_rx_callback(void(*cb)(uint32_t rx_addr, uint32_t tx_addr, uint32_t len, unsigned char rx_invalid));
EXTERNC int hal_sco_intersys_deregister_rx_callback(void(*cb)(uint32_t rx_addr, uint32_t tx_addr, uint32_t len, unsigned char rx_invalid));
EXTERNC void hal_sco_intersys_start(void);
EXTERNC void hal_sco_intersys_stop(void);
EXTERNC void hal_sco_over_intersys_init(void);
namespace smf {
	bool sco_init() {
		hal_sco_over_intersys_init();
		return true;
	}
	bool sco_start(void(*cb)(uint32_t rx_addr, uint32_t tx_addr, uint32_t len, unsigned char rx_invalid)) {
		dbgDevPPL(cb);
		returnIfErrC(false, !cb);
		auto rst = hal_sco_intersys_register_rx_callback(cb);
		returnIfErrCS(false, rst, "%d", rst);
		hal_sco_intersys_start();
		return true;
	}
	bool sco_stop(void(*cb)(uint32_t rx_addr, uint32_t tx_addr, uint32_t len, unsigned char rx_invalid)) {
		dbgDevPPL(cb);
		hal_sco_intersys_stop();
		auto rst = hal_sco_intersys_deregister_rx_callback(cb);
		returnIfErrCS(false, rst, "%d", rst);
		return true;
	}
}
