#include "SmfPorting.h"
namespace smf {
	bool sco_init() {
		return false;
	}
	bool sco_start(void(*cb)(uint32_t rx_addr, uint32_t tx_addr, uint32_t len)){
		return false;
	}
	bool sco_stop(void(*cb)(uint32_t rx_addr, uint32_t tx_addr, uint32_t len)) {
		return false;
	}
}
