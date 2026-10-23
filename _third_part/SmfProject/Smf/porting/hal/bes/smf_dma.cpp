#include "SmfPortingDevice.h"
#include "hal_dma.h"

#ifdef CHIP_SUBSYS_BTH
#include "hal_cmu.h"
#endif

namespace smf {
    uint8_t smf_dma_get_intcodec_halid(MediaDevice_e dev) {
        switch (dev) {
        case MEDIA_DEV_INTCODEC_RX: return HAL_AUDMA_CODEC_RX;
        case MEDIA_DEV_INTCODEC_TX: return HAL_AUDMA_CODEC_TX;
#ifdef SMF_USING_CODEC2
        case MEDIA_DEV_INTCODEC_RX2: return HAL_AUDMA_CODEC_RX2;
        case MEDIA_DEV_INTCODEC_TX2: return HAL_AUDMA_CODEC_TX2;
#endif
#ifdef SMF_USING_CODEC3
        case MEDIA_DEV_INTCODEC_RX3: return HAL_AUDMA_CODEC_RX3;
        case MEDIA_DEV_INTCODEC_TX3: return HAL_AUDMA_CODEC_TX3;
#endif
        default:
            break;
        }
        return 0;
    }

    uint8_t smf_dma_cmu_config(bool on) {
#if defined(CHIP_SUBSYS_BTH) && defined(SMF_CODEC_CLK_SEL_AON)
        if(on) {
            hal_cmu_clock_enable(HAL_CMU_MOD_H_CODEC);
            hal_cmu_reset_clear(HAL_CMU_MOD_H_CODEC);
        }else{
            hal_cmu_reset_set(HAL_CMU_MOD_H_CODEC);
            hal_cmu_clock_disable(HAL_CMU_MOD_H_CODEC);   
        }
#endif
        return 0;
    }
}