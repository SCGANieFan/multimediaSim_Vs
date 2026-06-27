/*
 * SmfPorting.h
 *
 *  Created on: 2024/8/22
 *      Author: wangjg
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>

namespace smf {
	uint32_t get_btpcm_reg0_addr(void);
	uint32_t get_btpcm_reg1_addr(void);

	void smf_adapter_bt_drv_reg_op_enable_dma_tc(uint8_t adma_ch, uint32_t dma_base);
	void smf_adapter_bt_drv_reg_op_disable_dma_tc(uint8_t adma_ch, uint32_t dma_base);
	void smf_adapter_bt_drv_reg_op_dma_tc_clkcnt_get_by_ch(uint32_t* btclk, uint16_t* btcnt, uint8_t adma_ch, uint32_t dma_base);
	void smf_adapter_btdrv_syn_clr_trigger(uint8_t trig_route);
    uint32_t smf_adapter_btdrv_syn_get_cis_curr_time(void);
    void smf_adapter_btdrv_enable_playback_triggler(uint8_t triggle_mode);
	uint32_t smf_adapter_bt_syn_ble_bt_time_to_bts(uint32_t hs, uint16_t hus);
	void smf_adapter_bt_syn_ble_set_tg_ticks(uint32_t val,uint8_t trig_route);
}
