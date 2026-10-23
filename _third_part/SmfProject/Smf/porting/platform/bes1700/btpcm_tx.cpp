#include "stdint.h"
#include "smf_common.h"

#define BTDIGITAL_REG(a) (*(volatile uint32_t *)(a))
#define SCO_TX_FIFO_BASE (0x6D020000)
#define BT_E_SCOCURRENTTXPTR_ADDR 0x6d040000+0x618
#define EM_SHIFT_EXT 1
namespace smf {
    uint32_t get_btpcm_rx_reg0_addr() {
        uint32_t regoffset0 = (BTDIGITAL_REG(BT_E_SCOCURRENTTXPTR_ADDR + 4) & 0x3fff) << (2 + EM_SHIFT_EXT);
        return SCO_TX_FIFO_BASE + regoffset0;
    }
    uint32_t get_btpcm_rx_reg1_addr() {
        uint32_t regoffset1 = ((BTDIGITAL_REG(BT_E_SCOCURRENTTXPTR_ADDR + 4) >> 16) & 0x3fff) << (2 + EM_SHIFT_EXT);
        return SCO_TX_FIFO_BASE + regoffset1;
    }
    uint32_t get_btpcm_reg0_addr() {
        uint32_t regoffset0 = (BTDIGITAL_REG(BT_E_SCOCURRENTTXPTR_ADDR) & 0x3fff) << (2 + EM_SHIFT_EXT);
        return SCO_TX_FIFO_BASE + regoffset0;
    }
    uint32_t get_btpcm_reg1_addr() {
        uint32_t regoffset1 = ((BTDIGITAL_REG(BT_E_SCOCURRENTTXPTR_ADDR) >> 16) & 0x3fff) << (2 + EM_SHIFT_EXT);
        return SCO_TX_FIFO_BASE + regoffset1;
    }
}