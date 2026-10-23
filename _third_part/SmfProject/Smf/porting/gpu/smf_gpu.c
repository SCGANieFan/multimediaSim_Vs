/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#include "plat_addr_map.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_sysfreq.h"
#include "hal_location.h"
#include "hal_iomux.h"
#include "hal_dma.h"
#include "vg_lite.h"

#include "smf_api_gpu.h"


#define PI              3.1415926f
#define DEG2RAD(d)      ((float)d*PI/180.0f)
#define RAD2DEG(r)      ((float)r*180.0f/PI)

#define GPU_TEST_DEBUG
#ifdef GPU_TEST_DEBUG
#define GPU_DBG(...)  TR_INFO( TR_MOD(TEST), "//GPU-TEST: "  __VA_ARGS__)
#else
#define GPU_DBG(...)
#endif
#define GPU_LOG(...)  TR_INFO( TR_MOD(TEST), "//GPU-TEST: "  __VA_ARGS__)
#define GPU_ERR(...)  TR_ERROR( TR_MOD(TEST), "//GPU-TEST: "  __VA_ARGS__)

static char *error_type[] =
{
    "VG_LITE_SUCCESS",
    "VG_LITE_INVALID_ARGUMENT",
    "VG_LITE_OUT_OF_MEMORY",
    "VG_LITE_NO_CONTEXT",
    "VG_LITE_TIMEOUT",
    "VG_LITE_OUT_OF_RESOURCES",
    "VG_LITE_GENERIC_IO",
    "VG_LITE_NOT_SUPPORT",
};
#define IS_ERROR(status)         (status > 0 && status <= 7)
#define CHECK_ERROR(Function) \
    do { \
        error = (vg_lite_error_t)Function; \
        if (IS_ERROR(error)) \
        { \
            ASSERT(0, "[%s: %d] failed, error type is %s\n", \
                __func__, __LINE__, error_type[error]); \
        } \
    } while (0)


static bool gpu_inited = false;
#define CHECK_GPU_INIT \
    do { \
        if (!gpu_inited) { \
            GPU_LOG("NOT initialized!"); \
            return -1; \
        } \
    } while (0)

static int gpu_width = 480;
static int gpu_height = 480;
static int gpu_format = VG_LITE_RGBA8888;

static vg_lite_filter_t gpu_filter = VG_LITE_FILTER_LINEAR;
static vg_lite_blend_t gpu_blend = VG_LITE_BLEND_SRC_OVER;

static vg_lite_buffer_t vg_buf;

int gpu_format_bpp(int format)
{
    switch(format) {
        case VG_LITE_A4:
            return 4;
        case VG_LITE_RGBA2222:
        case VG_LITE_BGRA2222:
        case VG_LITE_ABGR2222:
        case VG_LITE_ARGB2222:
        case VG_LITE_A8:
        case VG_LITE_L8:
            return 8;
        case VG_LITE_RGB565:
        case VG_LITE_BGR565:
        case VG_LITE_RGBA4444:
        case VG_LITE_BGRA4444:
        case VG_LITE_BGRA5551:
        case VG_LITE_ABGR4444:
        case VG_LITE_ARGB4444:
        case VG_LITE_ABGR1555:
        case VG_LITE_RGBA5551:
        case VG_LITE_ARGB1555:
        case VG_LITE_YUY2:
        case VG_LITE_YUYV:
            return 16;
        case VG_LITE_RGB888:
        case VG_LITE_BGR888:
        case VG_LITE_ABGR8565:
        case VG_LITE_BGRA5658:
        case VG_LITE_ARGB8565:
        case VG_LITE_RGBA5658:
            return 24;
        default:
        case VG_LITE_RGBA8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_RGBX8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_ABGR8888:
        case VG_LITE_ARGB8888:
        case VG_LITE_XBGR8888:
        case VG_LITE_XRGB8888:
            return 32;
    }
}

char *gpu_format_str(int format)
{
    switch (format) {
        case VG_LITE_RGBA8888:
            return "RGBA8888";
        case VG_LITE_BGRA8888:
            return "BGRA8888";
        case VG_LITE_RGBX8888:
            return "RGBX8888";
        case VG_LITE_BGRX8888:
            return "BGRX8888";
        case VG_LITE_ARGB8888:
            return "ARGB8888";
        case VG_LITE_ABGR8888:
            return "ABGR8888";
        case VG_LITE_XRGB8888:
            return "XRGB8888";
        case VG_LITE_XBGR8888:
            return "XBGR8888";
        case VG_LITE_RGB888:
            return "RGB888";
        case VG_LITE_BGR888:
            return "BGR888";
        case VG_LITE_RGBA5658:
            return "RGBA5658";
        case VG_LITE_BGRA5658:
            return "BGRA5658";
        case VG_LITE_ARGB8565:
            return "ARGB8565";
        case VG_LITE_ABGR8565:
            return "ABGR8565";
        case VG_LITE_RGB565:
            return "RGB565";
        case VG_LITE_BGR565:
            return "BGR565";
        case VG_LITE_RGBA5551:
            return "RGBA5551";
        case VG_LITE_BGRA5551:
            return "BGRA5551";
        case VG_LITE_ARGB1555:
            return "ARGB1555";
        case VG_LITE_ABGR1555:
            return "ABGR1555";
        case VG_LITE_RGBA4444:
            return "RGBA4444";
        case VG_LITE_BGRA4444:
            return "BGRA4444";
        case VG_LITE_ARGB4444:
            return "ARGB4444";
        case VG_LITE_ABGR4444:
            return "ABGR4444";
        case VG_LITE_RGBA2222:
            return "RGBA2222";
        case VG_LITE_BGRA2222:
            return "BGRA2222";
        case VG_LITE_ARGB2222:
            return "ARGB2222";
        case VG_LITE_ABGR2222:
            return "ABGR2222";
        case VG_LITE_A4:
            return "A4";
        case VG_LITE_A8:
            return "A8";
        case VG_LITE_L8:
            return "L8";
        case VG_LITE_YUY2:
            return "YUY2";
        case VG_LITE_YUYV:
            return "YUYV";
        default:
            return "ERR";
    }
    return "ERR";
}

char *gpu_blend_str(int blend)
{
    switch (blend) {
        case VG_LITE_BLEND_SRC_OVER:
            return "SRC_OVER";
        case VG_LITE_BLEND_SRC_IN:
            return "SRC_IN";
        case VG_LITE_BLEND_DST_OVER:
            return "DST_OVER";
        case VG_LITE_BLEND_DST_IN:
            return "DST_IN";
        case VG_LITE_BLEND_MULTIPLY:
            return "MULTIPLY";
        case VG_LITE_BLEND_ADDITIVE:
            return "ADDITIVE";
        case VG_LITE_BLEND_SUBTRACT:
            return "SUBTRACT";
        case VG_LITE_BLEND_NORMAL_LVGL:
            return "NORMAL_LVGL";
        case VG_LITE_BLEND_ADDITIVE_LVGL:
            return "ADDITIVE_LVGL";
        case VG_LITE_BLEND_SUBTRACT_LVGL:
            return "SUBTRACT_LVGL";
        case VG_LITE_BLEND_MULTIPLY_LVGL:
            return "MULTIPLY_LVGL";
        default:
            return "NONE";
    }
}

int gpu_test_open(int width, int height, int format, void *render_buf)
{
    int gpu_stride;

    vg_lite_error_t error = VG_LITE_SUCCESS;

    if (gpu_inited) {
        GPU_LOG("already initialized, do nothing.");
        return error;
    }

    if (width > GPU_TEST_MAX_WIDTH) {
        GPU_ERR("width must <= %d.", GPU_TEST_MAX_WIDTH);
        return -1;
    }

    if (height > GPU_TEST_MAX_HEIGHT) {
        GPU_ERR("height must <= %d.", GPU_TEST_MAX_HEIGHT);
        return -1;
    }

    if (gpu_format_bpp(format) == 0) {
        GPU_ERR("color format [%d] is not supported!", format);
        return -1;
    }

    gpu_width = width;
    gpu_height = height;
    gpu_format = format;
    gpu_stride = width * gpu_format_bpp(format) / 8;

    //CHECK_ERROR(vg_lite_init(gpu_width, gpu_height));

    memset(&vg_buf, 0, sizeof(vg_buf));

    vg_buf.format = (vg_lite_buffer_format_t)gpu_format;
    vg_buf.width = (int32_t) gpu_width;
    vg_buf.height = (int32_t) gpu_height;
    vg_buf.stride = gpu_stride;
    vg_buf.memory = render_buf;
    vg_buf.address = ADDR_CPU_TO_DEV((uint32_t)render_buf);

    if (error == VG_LITE_SUCCESS) {
        gpu_inited = true;
        GPU_LOG("initialized.");
    }

    return error;
}

int gpu_test_close(void)
{
    CHECK_GPU_INIT;

    //vg_lite_close();

    gpu_inited = false;

    GPU_LOG("closed.");
    return 0;
}

void gpu_test_set_filter(vg_lite_filter_t filter)
{
    if (filter == VG_LITE_FILTER_LINEAR || filter == VG_LITE_FILTER_BI_LINEAR) {
        gpu_filter = filter;
    } else {
        gpu_filter = VG_LITE_FILTER_POINT;
    }

    GPU_LOG("Set default filter to %d (0x%x)", gpu_filter, gpu_filter);
}

void gpu_test_set_blend(vg_lite_blend_t blend)
{
    gpu_blend = blend;
    GPU_LOG("Set default blend mode to %d (0x%x)", gpu_blend, gpu_blend);
}

int gpu_test_get_bpp(void)
{
    return gpu_format_bpp(gpu_format);
}

static int circle_coords_calc(float center_x, float center_y, float radius, float angle,
                         float *out_x, float *out_y)
{
    float x, y;

    x = center_x - radius * cosf(angle * PI / 180.0f);
    y = center_y - radius * sinf(angle * PI / 180.0f);

    if (out_x)
        *out_x = x;
    if (out_y)
        *out_y = y;
    return 0;
}

static void correct_rotate_position(float *x, float *y, float w, float h, float angle)
{
    float center_x = *x + w / 2.0f;
    float center_y = *y + h / 2.0f;
    float radius = sqrtf(w * w + h * h) / 2.0f;
    circle_coords_calc(center_x, center_y, radius, 45.0+angle, x, y);
}

int gpu_test_blit(void *src, int src_width, int src_height, int src_format,
                  void *dst, int dst_width, int dst_height, int dst_format,
                  float scale, int rotate)
{
    uint32_t start = hal_fast_sys_timer_get();
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t matrix;
    vg_lite_buffer_t src_buf;
    vg_lite_buffer_t dst_buf;
    float sw, sh;
    float tx, ty;

    CHECK_GPU_INIT;

    vg_lite_identity(&matrix);

    sw = (float)src_width * scale;
    sh = (float)src_height * scale;

    tx = ((float)dst_width - sw) / 2.0f;
    ty = ((float)dst_height - sh) / 2.0f;

    if (rotate > 0)
        correct_rotate_position(&tx, &ty, sw, sh, rotate);

    vg_lite_translate(tx, ty, &matrix);

    vg_lite_scale(scale, scale, &matrix);

    if (rotate > 0)
        vg_lite_rotate((float)rotate, &matrix);

    memset(&src_buf, 0, sizeof(vg_lite_buffer_t));

    src_buf.format = (vg_lite_buffer_format_t)src_format;
    src_buf.width = src_width;
    src_buf.height = src_height;
    src_buf.stride = src_width * gpu_format_bpp(src_format) / 8;
    src_buf.memory = src;
    src_buf.address = ADDR_CPU_TO_DEV((uint32_t)src);

    memset(&dst_buf, 0, sizeof(vg_lite_buffer_t));

    dst_buf.format = (vg_lite_buffer_format_t)dst_format;
    dst_buf.width = dst_width;
    dst_buf.height = dst_height;
    dst_buf.stride = dst_width * gpu_format_bpp(dst_format) / 8;
    dst_buf.memory = dst;
    dst_buf.address = ADDR_CPU_TO_DEV((uint32_t)dst);

    CHECK_ERROR(vg_lite_blit(&dst_buf, &src_buf, &matrix, gpu_blend, 0, gpu_filter));
    CHECK_ERROR(vg_lite_finish());

    GPU_LOG("%s, scale %d%%, rotate %d, [%p,%d]->[%p,%d] used %uus",
            (gpu_blend == VG_LITE_BLEND_NONE) ? "blit" : "blend",
            (uint32_t)(scale*100), rotate,
            src, src_format, dst, dst_format,
            FAST_TICKS_TO_US(hal_fast_sys_timer_get() - start));
    return error;
}


int gpu_test_set_buf(void *render_buf)
{
    CHECK_GPU_INIT;

    vg_buf.memory = render_buf;
    vg_buf.address = ADDR_CPU_TO_DEV((uint32_t)render_buf);

    GPU_LOG("use %p as render buf", render_buf);
    return 0;
}


int gpu_test_draw_nv12(uint8_t *img_data, int img_w, int img_h,
                       int target_x, int target_y, float scale)
{
    uint32_t start = hal_fast_sys_timer_get();
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t matrix;
    vg_lite_buffer_t src_buf;
    void *uv_data = (void *)(img_data + img_w * img_h);

    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_YUV_INPUT)) {
        GPU_LOG("%s: not support!", __func__);
        return VG_LITE_NOT_SUPPORT;
    }

    CHECK_GPU_INIT;

    memset(&src_buf, 0, sizeof(vg_lite_buffer_t));
    src_buf.width = img_w;
    src_buf.height = img_h;
    src_buf.format = VG_LITE_NV12;
    src_buf.stride = img_w;
    src_buf.memory = (void *)img_data;
    src_buf.address = ADDR_CPU_TO_DEV((uint32_t)img_data);
    src_buf.yuv.swizzle = VG_LITE_SWIZZLE_UV;
    src_buf.yuv.uv_memory = uv_data;
    src_buf.yuv.uv_planar = ADDR_CPU_TO_DEV((uint32_t)uv_data);
    src_buf.yuv.uv_stride = img_w;
    src_buf.yuv.uv_height = img_h / 2;
    src_buf.yuv.alpha_stride = img_w;

    vg_lite_identity(&matrix);
    vg_lite_translate(target_x, target_y, &matrix);
    vg_lite_scale(scale, scale, &matrix);

    CHECK_ERROR(vg_lite_blit(&vg_buf, &src_buf, &matrix, gpu_blend, 0, gpu_filter));
    CHECK_ERROR(vg_lite_finish());

    GPU_LOG("draw nv12 image %p->%p used %uus", src_buf.memory, vg_buf.memory,
                        FAST_TICKS_TO_US(hal_fast_sys_timer_get() - start));
    return error;
}