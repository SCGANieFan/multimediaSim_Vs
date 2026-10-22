/***************************************************************************
 *
 * Copyright 2015-2023 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "cmsis.h"
#include "hal_timer.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_cache.h"
#include "hal_location.h"
#include "hal_trace.h"
#include "hal_sysfreq.h"
#include "hal_cmu.h"
#include "hal_psc.h"
#include "lcdc_drv.h"

#define DISPLAY_RM69330
//#define DISPLAY_GC9503NP
//#define DISPLAY_HX8394F
//#define DISPLAY_HX8394F720
//#define DISPLAY_HX8399C
//#define DISPLAY_FL7703NI
//#define DISPLAY_ST7701SN
//#define DISPLAY_ST7703DS
//#define DISPLAY_ST7785M

#ifndef CONFIG_LCDC_DSI
#define CONFIG_LCDC_DSI
#endif
//#define CONFIG_LCDC_USE_DSI0
//#define CONFIG_LCDC_USE_DSI1
//#define CONFIG_LCDC_DUAL_DSI
//#define CONFIG_DSI_VIDEO_MODE
//#define CONFIG_DSI_2LANE_MODE
//#define CONFIG_DSI_4LANE_MODE
//#define CONFIG_LCD_RGB565
//#define CONFIG_LCD_RGB888

#include "lcd_conf.h"
#include "lcd_cmds.h"

//#define TEST_YUV422_UYVY
//#define TEST_YUV422_I422
// #define TEST_YUV420_I420
//#define TEST_YUV420_NV12
//#define TEST_YUV420_P16

/* framebuffer size */
#define FB_WIDTH                    ((LCD_WIDTH + 15) / 16 * 16)
#define FB_HEIGHT                   LCD_HEIGHT

/* DSI0 layer settings */
// #if defined(TEST_YUV422_UYVY)
// #define LAYER1_FORMAT               LCDC_CFMT_YUV422_UYVY
// #define LAYER1_STRIDE               (FB_WIDTH * 2)
// #elif defined(TEST_YUV422_I422)
// #define LAYER1_FORMAT               LCDC_CFMT_YUV422_I422
// #define LAYER1_STRIDE               FB_WIDTH
// #elif defined(TEST_YUV420_I420)
// #define LAYER1_FORMAT               LCDC_CFMT_YUV420_I420
// #define LAYER1_STRIDE               FB_WIDTH
// #elif defined(TEST_YUV420_NV12)
// #define LAYER1_FORMAT               LCDC_CFMT_YUV420_NV12
// #define LAYER1_STRIDE               FB_WIDTH
// #elif defined(TEST_YUV420_P16)
// #define LAYER1_FORMAT               LCDC_CFMT_YUV420_P16
// #define LAYER1_STRIDE               FB_WIDTH
// #elif defined(CONFIG_LCD_RGB565)
// #define LAYER1_FORMAT               LCDC_CFMT_RGB565
// #define LAYER1_STRIDE               (FB_WIDTH * 2)
// #elif defined(CONFIG_LCD_RGB888)
// #define LAYER1_FORMAT               LCDC_CFMT_RGB888
// #define LAYER1_STRIDE               (FB_WIDTH * 3)
// #else
// #define LAYER1_FORMAT               LCDC_CFMT_RGBA8888
// #define LAYER1_STRIDE               (FB_WIDTH * 4)
// #endif
// #define LAYER1_SIZE                 (FB_WIDTH * FB_HEIGHT * 4)


/* DSI1 layer settings */
#define LAYER2_FORMAT               LCDC_CFMT_RGBA8888
#define LAYER2_STRIDE               (FB_WIDTH * 4)
#define LAYER2_SIZE                 (FB_WIDTH * FB_HEIGHT * 4)

#define NCOLORS 10

POSSIBLY_UNUSED
static uint32_t test_colors_RGBA8888[NCOLORS] = {
    0xffffffff, //white
    0xff000000, //black
    0xffff0000, //blue
    0xff00ff00, //green
    0xff0000ff, //red
    0xff90ee90, //light green
    0xffa52a2a, //brown
    0xffadd8e6, //light blue
    0xffee82ee, //violet
    0xff00ffff, //yellow
};

POSSIBLY_UNUSED
static uint16_t test_colors_RGB565[NCOLORS] = {
    0xffff, //white
    0x0000, //black
    0x001f, //red
    0x07e0, //green
    0xf800, //blue
    0x0011, //dark red
    0x8410, //gray
    0x06bf, //gold
    0x8010, //purple
    0x07ff, //yellow
};

POSSIBLY_UNUSED
static uint32_t test_colors_UYVY[NCOLORS] = {
    0xff80ff80, //white
    0x00800080, //black
    0x127412ff, //blue
    0xb60cb61d, //green
    0x36ff3662, //red
    0x47524759, //dark green
    0x327a32bd, //brown
    0xd788d768, //light blue
    0xa0b1a0a9, //violet
    0xec8bec00, //yellow
};

static struct lcdc_config_t lcdc_config = {
#ifdef CONFIG_DSI_VIDEO_MODE
    .mode = LCDC_VIDEO_MODE,
#else
    .mode = LCDC_COMMAND_MODE,
#endif
#ifdef CONFIG_LCDC_DUAL_DSI
    .bus = LCDC_BUS_DUAL_DSI,
    .bus_port0 = HAL_DSI_ID_0,
    .bus_port1 = HAL_DSI_ID_1,
#else
    .bus = LCDC_BUS_DSI,
#ifdef CONFIG_LCDC_USE_DSI1
    .bus_port0 = HAL_DSI_ID_1,
    .bus_port1 = HAL_DSI_ID_1,
#else
    .bus_port0 = HAL_DSI_ID_0,
    .bus_port1 = HAL_DSI_ID_0,
#endif
#endif
#ifdef DSIPLL_MHZ
    .bus_mhz = DSIPLL_MHZ,
#else
    .bus_mhz = 400,
#endif
#ifdef CONFIG_DSI_4LANE_MODE
    .bus_lane = 4,
#elif defined(CONFIG_DSI_2LANE_MODE)
    .bus_lane = 2,
#else
    .bus_lane = 1,
#endif
    .pix_mhz = 24,

    .lcd = {
        .width  = LCD_WIDTH,
        .height = LCD_HEIGHT,
        .hfp    = LCD_HFP,
        .hbp    = LCD_HBP,
        .hsync  = LCD_HSYNC,
        .vfp    = LCD_VFP,
        .vbp    = LCD_VBP,
        .vsync  = LCD_VSYNC,
#ifdef CONFIG_LCD_RGB565
        .color_bits = 16,
#else
        .color_bits = 24,
#endif
#ifdef CONFIG_LCD_SWAP_RB
        .swap_rb = true,
#else
        .swap_rb = false,
#endif
    },

    .te_pin = -1,

    .framedone_cb = NULL,
    .vsync_cb = NULL,
    .error_cb = NULL,
};

#if defined(DDR_ENABLE)
#define FB_LOC      DDR_BSS_LOC
#elif defined(PSRAM_ENABLE)
#define FB_LOC      PSRAM_NC_BSS_LOC
#else
#define FB_LOC
#endif

// static uint8_t ALIGNED(64) frame_buf1[LAYER1_SIZE] FB_LOC;
#ifdef CONFIG_LCDC_DUAL_DSI
static uint8_t ALIGNED(64) frame_buf2[LAYER2_SIZE] FB_LOC;
#endif

static uint32_t frame_start = 0;
static uint32_t frame_count = 0;
static bool framedone_irq_get = false;
static bool te_irq_get = false;

/******************************** LCD Init ****************************************/

POSSIBLY_UNUSED
static int lcd_read_id(int dsi, uint32_t *id)
{
    int ret = 0;
    uint8_t *ids = (uint8_t *)id;
    memset(id, 0, sizeof(uint32_t));
    ret = lcdc_read_cmd(dsi, 0xda, &ids[0], 1);
    if (ret < 0)
        return -1;
    ret = lcdc_read_cmd(dsi, 0xdb, &ids[1], 1);
    if (ret < 0)
        return -1;
    ret = lcdc_read_cmd(dsi, 0xdc, &ids[2], 1);
    if (ret < 0)
        return -1;
    return 0;
}

POSSIBLY_UNUSED
static int lcd_read_status(int dsi, uint8_t *st)
{
    int ret = 0;
    ret = lcdc_read_cmd(dsi, 0x0a, st, 1);
    if (ret < 0)
        return -1;
    return 0;
}

static int lcd_panel_init(int dsi)
{
    uint8_t lcd_status = 0;
    uint32_t lcd_id = 0;
    struct lcd_command *cmds = lcd_init_cmds;
    int count = sizeof(lcd_init_cmds) / sizeof(struct lcd_command);
#ifdef SIMU
    return 0;
#endif

    if (lcd_read_status(dsi, &lcd_status) < 0) {
        TRACE(0, "Failed to read LCD power status.");
        return -1;
    }
    TRACE(0, "LCD power status: %02x", lcd_status);

    if (lcd_read_id(dsi, &lcd_id) < 0) {
        TRACE(0, "Failed to read LCD id.");
        return -1;
    }
    TRACE(0, "LCD id: 0x%06x", lcd_id);

    for (int i = 0; i < count; i++) {
        if (cmds[i].param_cnt > 0)
            lcdc_send_cmd_list(dsi, cmds[i].cmd, cmds[i].param_cnt, cmds[i].param_data);
        else
            lcdc_send_cmd(dsi, cmds[i].cmd);
        if (cmds[i].delay_ms > 0)
            hal_sys_timer_delay(MS_TO_TICKS(cmds[i].delay_ms));
    }

    TRACE(0, "LCD [%s] is initialized.", LCD_NAME);
    return 0;
}

static void lcd_power_on(void)
{
#ifndef SIMU
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux[] = {
        {(enum HAL_IOMUX_PIN_T)LCD_RST_PIN, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
    };

#ifdef LCD_LED_PIN
    pinmux[0].pin = LCD_LED_PIN;
    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
    hal_gpio_pin_set_dir(LCD_LED_PIN, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set(LCD_LED_PIN);
#endif
#ifdef LCD_1V8_PIN
    pinmux[0].pin = (enum HAL_IOMUX_PIN_T)LCD_1V8_PIN;
    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
    hal_gpio_pin_set_dir(LCD_1V8_PIN, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set(LCD_1V8_PIN);
#endif
#ifdef LCD_2V8_PIN
    pinmux[0].pin = (enum HAL_IOMUX_PIN_T)LCD_2V8_PIN;
    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
    hal_gpio_pin_set_dir(LCD_2V8_PIN, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set(LCD_2V8_PIN);
#endif
#ifdef LCD_3V3_PIN
    pinmux[0].pin = (enum HAL_IOMUX_PIN_T)LCD_3V3_PIN;
    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
    hal_gpio_pin_set_dir(LCD_3V3_PIN, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set(LCD_3V3_PIN);
#endif
    pinmux[0].pin = (enum HAL_IOMUX_PIN_T)LCD_RST_PIN;
    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
    hal_gpio_pin_set_dir(LCD_RST_PIN, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set(LCD_RST_PIN);
#ifdef LCD_RST_PIN2
    pinmux[0].pin = (enum HAL_IOMUX_PIN_T)LCD_RST_PIN2;
    hal_iomux_init(pinmux, ARRAY_SIZE(pinmux));
    hal_gpio_pin_set_dir(LCD_RST_PIN2, HAL_GPIO_DIR_OUT, 0);
    hal_gpio_pin_set(LCD_RST_PIN2);
#endif
    hal_sys_timer_delay(MS_TO_TICKS(10));
    hal_gpio_pin_clr(LCD_RST_PIN);
#ifdef LCD_RST_PIN2
    hal_gpio_pin_clr(LCD_RST_PIN2);
#endif
    hal_sys_timer_delay(MS_TO_TICKS(1));
    hal_gpio_pin_set(LCD_RST_PIN);
#ifdef LCD_RST_PIN2
    hal_gpio_pin_set(LCD_RST_PIN2);
#endif
    hal_sys_timer_delay(MS_TO_TICKS(10));
#endif
}

/*********************************** LCDC control **************************************/

static void lcdc_framedone_cb(void)
{
    framedone_irq_get = true;
#ifdef CONFIG_DSI_VIDEO_MODE
    TRACE(0, "frame %u transfer done, used %uus", frame_count,
              FAST_TICKS_TO_US(hal_fast_sys_timer_get() - frame_start));
    frame_count++;

    TRACE(0, "frame %u transfer start", frame_count);
    frame_start = hal_fast_sys_timer_get();
#endif
}

POSSIBLY_UNUSED
static int lcdc_wait_framedone_irq(void)
{
    // TRACE(0, "wait for framedone irq...");
    while (!framedone_irq_get && FAST_TICKS_TO_MS(hal_fast_sys_timer_get()-frame_start) < 500)
        hal_sys_timer_delay_us(10);
    framedone_irq_get = false;
    if (FAST_TICKS_TO_MS(hal_fast_sys_timer_get()-frame_start) >= 500)
        return -1;
    return 0;
}

static void lcdc_vsync_cb(void)
{
    te_irq_get = true;
}

POSSIBLY_UNUSED
static void lcdc_wait_te_irq(void)
{
#ifndef SIMU
    // TRACE(0, "wait for TE irq...");
    while (!te_irq_get)
        hal_sys_timer_delay_us(10);
    te_irq_get = false;
#endif
}

static void lcdc_dsi_init(void)
{

    lcd_power_on();

    if (lcdc_config.mode == LCDC_COMMAND_MODE && lcdc_config.te_pin < 0) {
#ifdef LCD_TE_PIN
        lcdc_config.te_pin = LCD_TE_PIN;
#else
        lcdc_config.te_pin = hal_iomux_get_dsi_te_pin();
#endif
    }
    lcdc_exit();
    lcdc_init(&lcdc_config);

    lcdc_register_framedone_cb(lcdc_framedone_cb);
    lcdc_register_vsync_cb(lcdc_vsync_cb);

    if (lcd_panel_init(lcdc_config.bus_port0))
        ASSERT(0, "LCD0 init failed!");
    if (lcdc_config.bus == LCDC_BUS_DUAL_DSI &&
        lcd_panel_init(lcdc_config.bus_port1))
        ASSERT(0, "LCD1 init failed!");

    frame_count = 0;
    // TRACE(0, "lcdc initialized.");
}

static void lcdc_dsi_config(uint32_t buffer, uint32_t width, uint32_t height, enum hal_lcdc_color_format_e format, uint32_t stride)
{
    struct lcdc_layer_config_t config;

    memset(&config, 0, sizeof(config));
    config.width = width;
    config.height = height;

    config.format = format;
    config.stride = stride;
    config.buffer = (uint8_t*)buffer;
    config.enable = true;
    lcdc_layer_config(LCDC_LAYER_DMA, &config);


#ifdef CONFIG_LCDC_DUAL_DSI
    config.format = LAYER2_FORMAT;
    config.stride = LAYER2_STRIDE;
    config.buffer = frame_buf2;
    config.enable = true;
    lcdc_layer_config(LCDC_LAYER_TVG, &config);
    lcdc_layer_select(lcdc_config.bus_port0, 1 << LCDC_LAYER_DMA);
    lcdc_layer_select(lcdc_config.bus_port1, 1 << LCDC_LAYER_TVG);
#endif

    if(format == LCDC_CFMT_PALETTE_8) {
        lcdc_palette_enable(LCDC_CFMT_PALETTE_8, (const uint32_t *)((uint8_t*)buffer + width*height), 256);
    }
    lcdc_start();
}

POSSIBLY_UNUSED
static void lcdc_dsi_exit(void)
{
    lcdc_stop();
    lcdc_exit();
}

POSSIBLY_UNUSED
static int lcdc_frame_send(void)
{
    // TRACE(0, "frame %u transfer start", frame_count);

    // lcdc_wakeup();

    //lcdc_wait_te_irq();

    lcdc_xfer_frame();

    //frame_start = hal_fast_sys_timer_get();

    //if (lcdc_wait_framedone_irq()) {
    //    TRACE(0, "frame transfer failed!");
    //    return -1;
    //}

    //frame_count++;

    // lcdc_sleep();
    return 0;
}

#include "smf_api_display.h"
//#include "smf_debug.h"
static uint8_t _format = 0;
static uint8_t _alpha = 0;
static uint32_t _width = 0;
static uint32_t _height = 0;
static uint32_t _stride = 0;
WEAK void* smf_display_open(void* para) {
    lcdc_dsi_init();
    return (void*)1;
}
WEAK bool smf_display_close(void* handle) {
    lcdc_dsi_exit();
    return true;
}
WEAK bool smf_display_config(void* handle, smf_display_config_t* cfg) {
    uint8_t format = 0;
    uint32_t stride = 0;
    switch (cfg->format) {
    case i420:      format = LCDC_CFMT_YUV420_I420; stride = cfg->width; break;
    case nv12:      format = LCDC_CFMT_YUV420_NV12; stride = cfg->width; break;
    case i420p16:   format = LCDC_CFMT_YUV420_P16; stride = cfg->width; break;
    case i422:      format = LCDC_CFMT_YUV422_I422; stride = cfg->width; break;
    case uyvy:      format = LCDC_CFMT_YUV422_UYVY; stride = cfg->width * 2; break;
    //case vyuy:      format = LCDC_CFMT_YUV422_VYUY; stride = cfg->width * 2;break;
  
    case rgb565:    format = LCDC_CFMT_RGB565; stride = cfg->width * 2; break;
    case rgb888:    format = LCDC_CFMT_RGB888; stride = cfg->width * 3; break;
    case rgba8888:  format = LCDC_CFMT_RGBA8888; stride = cfg->width * 4; break;
    case pal8:      format = LCDC_CFMT_PALETTE_8; stride = cfg->width; break;
    default:
        TRACE(0, "%s()fmt(%u) error!", __func__, cfg->format);
        break;
    }
    _format = format;
    _alpha = cfg->alpha;
    _width = cfg->width;
    _height = cfg->height;
    _stride = stride;
    //dbgTestPXL("%p,w:%u,h:%u,fmt:%u/%u,stride:%u,alpha:%u", handle, _width, _height,cfg->format,_format,_stride, _alpha);
    return true;
}
WEAK bool smf_display_trigger(void* handle, void* fbaddr, uint32_t fbsize) {
    //dbgTestPXL("%p,%p,%u,w:%u,h:%u,fmt:%u,stride:%u,alpha:%u", handle, fbaddr, fbsize, _width, _height, _format, _stride, _alpha);
    lcdc_dsi_config((uint32_t)fbaddr, _width, _height, _format, _stride);
    lcdc_frame_send();
    return true;
}
