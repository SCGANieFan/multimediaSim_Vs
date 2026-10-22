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

#ifndef __SMF_GPU_H__
#define __SMF_GPU_H__
#include "smf_common.h"

#include "vg_lite.h"

#define GPU_TEST_MAX_WIDTH      2048
#define GPU_TEST_MAX_HEIGHT     2048


/***************************** Basic functions ********************************/

EXTERNC int gpu_format_bpp(int format);

EXTERNC char *gpu_format_str(int format);

EXTERNC char *gpu_blend_str(int blend);

/* Open GPU test library,
 * 1. Initialize vglite driver;
 * 2. Initialize a default render buffer for test;
 */
EXTERNC int gpu_test_open(int width, int height, int format, void *render_buf);

/* Close GPU test library */
EXTERNC int gpu_test_close(void);

/* Get bpp (bits per pixel) of the default render buffer */
EXTERNC int gpu_test_get_bpp(void);

/* Set the default filter */
EXTERNC void gpu_test_set_filter(vg_lite_filter_t filter);

/* Set the default blend mode */
EXTERNC void gpu_test_set_blend(vg_lite_blend_t blend);

/* Blit buffer data to another buffer, with more parameters */
EXTERNC int gpu_test_blit(void *src, int src_width, int src_height, int src_format,
                  void *dst, int dst_width, int dst_height, int dst_format,
                  float scale, int rotate);

EXTERNC int gpu_test_set_buf(void *render_buf);

EXTERNC int gpu_test_draw_nv12(uint8_t *img_data, int img_w, int img_h,int target_x, int target_y, float scale);
#endif
