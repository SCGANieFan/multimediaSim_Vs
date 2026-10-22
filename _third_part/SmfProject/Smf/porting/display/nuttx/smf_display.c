#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <nuttx/video/fb.h>
#include "smf_api_display.h"
#include "smf_api_gpu.h"
#include "hal_cache.h"
#include "hal_trace.h"

static struct fb_planeinfo_s _pinfo;
WEAK void* smf_display_open(void*para) {
    FAR const char* path = (const char*)para; //"/dev/fb0.1";
    int fd = open(path, O_RDWR);
    if (fd < 0) {
        TRACE(0, "%s: ERROR: Failed to open %s: %d,%d\n", __func__, path, errno, fd); 
        return 0;
    }
    return (void*)(fd + 1);
}
WEAK bool smf_display_close(void* handle) {
    if (!handle) {
        TRACE(0, "%s: ERROR: no handle: %d\n", __func__, errno);
        return false;
    }
    int fd = (int)handle - 1;
    close(fd);
    return true;
}
WEAK bool smf_display_config(void* handle, smf_display_config_t* cfg) {
    if (!handle) {
        TRACE(0, "%s: ERROR: no handle: %d\n", __func__, errno);
        return false;
    }
    int fd = (int)handle - 1;
    int ret = 0;
    uint8_t format = 0;
    uint8_t layer = cfg->layer;
    switch (cfg->format) {
    case i420:  format = (layer == 1) ? FB_FMT_I420 : FB_FMT_RGB24; break;
    case vyuy:  format = FB_FMT_VYUY; break;
    case nv12:  format = (layer == 1) ? FB_FMT_NV12 : FB_FMT_RGB24; break;
    case yuyv:  format = FB_FMT_YUYV; break;
    default:
        TRACE(0, "fmt(%u) error!", cfg->format);
        return false;
    }
    ///get video info
    struct fb_videoinfo_s vinfo;
    ret = ioctl(fd, FBIOGET_VIDEOINFO, (unsigned long)((uintptr_t)&vinfo));
    if (ret < 0) {
        TRACE(0, "%s: set video info fd %d failed: %d/%d\n", __func__, fd, errno, ret);
        return false;
    }
    TRACE(0, "VideoInfo:\n");
    TRACE(0, "      fmt: %u\n", vinfo.fmt);
    TRACE(0, "     xres: %u\n", vinfo.xres);
    TRACE(0, "     yres: %u\n", vinfo.yres);
    TRACE(0, "  nplanes: %u\n", vinfo.nplanes);
    int xres = cfg->width;
    int yres = cfg->height;
    int xoffset = (vinfo.xres - xres) > 0 ? (vinfo.xres - xres) / 2 : 0;
    int yoffset = (vinfo.yres - yres) > 0 ? (vinfo.yres - yres) / 2 : 0;
    vinfo.fmt = format;
    vinfo.xres = xres;
    vinfo.yres = yres;
#ifdef CONFIG_FB_SYNC
    ret = ioctl(fd, FBIO_WAITFORVSYNC, 0);
    if ((ret < 0) && (errno != ETIMEDOUT)) {
        TRACE(0, "%s: wait upon fd %d failed: %d/%d\n", __func__, fd, errno, ret);
        return false;
    }
#endif
    ///set video info
    ret = ioctl(fd, 0, &vinfo);
    if (ret < 0) {
        TRACE(0, "%s: reset layer config fd %d failed: %d/%d\n", __func__, fd, errno, ret);
        return false;
    }
    ///set overlay info
    struct fb_overlayinfo_s oinfo;
    oinfo.xoffset = xoffset;
    oinfo.yoffset = yoffset;
    ret = ioctl(fd, FBIOSET_AREA, &oinfo);
    if (ret < 0) {
        TRACE(0, "%s: set offset fd %d failed: %d/%d\n", __func__, fd, errno, ret);
        return false;
    }
    ///set blend
    uint8_t blend_conf[4];
    //  0: dsi0  1: dsi1    (not all chip support dsi1)
    blend_conf[0] = 0;
    //  0:  /* (1-a) * dma-pix + a * gra-pix */
    //  1:  /* (1-a) * dma-pix + 1 * gra-pix */
    //  2:  /* a * dma-pix + 1 * gra-pix */
    blend_conf[1] = 0;
    //  0: CONST_ALPHA
    //  1: PIXEL_ALPHA  (gra layer *.0)
    blend_conf[2] = 0;
    // alpha value: only invalid when const_alpha is used between 0~255
    blend_conf[3] = cfg->alpha;
    ret = ioctl(fd, 1, blend_conf);
    if (ret != OK) {
        TRACE(0, "%s: set blend upon fd %d failed: %d/%d\n", __func__, fd, errno, ret);
        return false;
    }
    ///get plane info
    ret = ioctl(fd, FBIOGET_PLANEINFO, (unsigned long)((uintptr_t)&_pinfo));
    if (ret < 0) {
        TRACE(0, "%s: set plane info fd %d failed: %d/%d\n", __func__, fd, errno, ret);
        return false;
    }
    return true;
}

typedef struct {
    uint8_t u;
    uint8_t v;
}uv_t;

static bool nv12_to_yuyv(void* idata, void* odata, int _w, int _h) {
    uint8_t* optr = (uint8_t*)odata;
    uint8_t* _yp = (uint8_t*)idata;

    uv_t* _uv = (uv_t*)(_yp + _w * _h);

    for (int i = 0; i < _h; i++) {
        for (int j = 0; j < _w; j += 2) {
            int idx = ((i >> 1) * (_w >> 1)) + (j >> 1);
            optr[0] = *_yp++;
            optr[1] = _uv[idx].u;
            optr[2] = *_yp++;
            optr[3] = _uv[idx].v;
            optr += 4;
        }
    }
    return true;
}

static bool nv12_to_rgb888_hw(void* input, void* output, struct fb_videoinfo_s *info) {

    struct fb_videoinfo_s *vinfo = info;
    gpu_test_open(vinfo->xres, vinfo->yres, VG_LITE_RGB888, NULL);
    gpu_test_set_blend(VG_LITE_BLEND_SRC_OVER);
    gpu_test_set_filter(VG_LITE_FILTER_LINEAR);
    gpu_test_set_buf(output);
    gpu_test_draw_nv12(input, vinfo->xres, vinfo->yres, 0, 0, 1.0f);
    gpu_test_close();
    return true;
}

WEAK bool smf_display_trigger(void* handle, void* fbaddr, uint32_t fbsize) {
    if (!handle) {
        TRACE(0, "%s: ERROR: no handle: %d\n", __func__, errno);
        return false;
    }
    int fd = (int)handle - 1;
    int ret = 0;
    //
    struct fb_videoinfo_s vinfo;
    ret = ioctl(fd, FBIOGET_VIDEOINFO, (unsigned long)((uintptr_t)&vinfo));
    if (ret == OK) {
        // TRACE(0, "%s:%d\n", __func__, __LINE__);
        // TRACE(0, "VideoInfo:\n");
        // TRACE(0, "      fmt: %u\n", vinfo.fmt);
        // TRACE(0, "     xres: %u\n", vinfo.xres);
        // TRACE(0, "     yres: %u\n", vinfo.yres);
        // TRACE(0, "  nplanes: %u\n", vinfo.nplanes);
        if (vinfo.fmt == FB_FMT_RGB24) {
            // nv12_to_yuyv(fbaddr, buf, vinfo.xres, vinfo.yres);
            if (ioctl(fd, FBIOGET_PLANEINFO, (unsigned long)((uintptr_t)&_pinfo)) < 0) {
                TRACE(0, "ERROR: ioctl(FBIOGET_PLANEINFO) failed\n");
                return false;
            }

            if (_pinfo.fblen < fbsize || !_pinfo.fbmem) {
                TRACE(0, "%s: ERROR: %d,%d,%p\n", _pinfo.fblen, fbsize, _pinfo.fbmem);
                return false;
            }
    
            nv12_to_rgb888_hw(fbaddr, _pinfo.fbmem, &vinfo);
            fbsize = vinfo.xres * vinfo.yres * 3;
            fbaddr = _pinfo.fbmem;
        }
    }
    {
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLOUT;
        // if render speed < transfering speed, it's not necessary wait upon fd by polling available buffer only if you are using double buffer
        // because once pandisplay is returned, it doesn't mean the frame is transfer done
        ret = poll(&pfd, 1, -1);
    }
    if (ret > 0) {
        _pinfo.fbmem = fbaddr;
        _pinfo.fblen = fbsize;
        _pinfo.yoffset = 0;
        ret = ioctl(fd, FBIOPAN_DISPLAY, (unsigned long)(uintptr_t)&_pinfo);
        if (ret < 0) {
            TRACE(0, "ERROR: ioctl(FBIOPAN_DISPLAY) failed: %d,%d\n", errno, ret);
            return false;
        }
    }
    return true;
}
