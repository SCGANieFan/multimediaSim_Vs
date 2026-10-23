export path_libs := $(path_root)/libs/$(SMF_PLATFORM)_nuttx/
export smfapps-mk := 
smfapps-mk += mmgr/
smfapps-mk += ssvc/
smfapps-mk += msvc/
smfapps-mk += camera/
smfapps-mk += camera/ap/
smfapps-mk += camera/isp/
# smfapps-mk += rtsp4/
# smfapps-mk += rtspclient/

export smfplugin-mk := 
smfplugin-mk += fifo/
smfplugin-mk += func/
smfplugin-mk += shm/
#smfplugin-mk += ipcfs/
smfplugin-mk += vpu_vc9000_ld/
smfplugin-mk += vpu_vc9000_le/
#smfplugin-mk += isp/
smfplugin-mk += imu/

export smftest-mk :=
#smftest-mk += bandwidth/

export smfporting-mk :=
smfporting-mk += hal/bes/
smfporting-mk += cpu/a55/
smfporting-mk += fs/std/
smfporting-mk += os/nuttx/
smfporting-mk += osapi/pthread/
smfporting-mk += ipc/openamp_imsvc/
smfporting-mk += dev/other/
smfporting-mk += algo/other/
# smfporting-mk += cmd/eshell/
export smfporting-ccflags :=

export smfdemo-mk :=
smfdemo-mk += camera/
smfdemo-mk += mmgr/
smfdemo-mk += ssvc/

# 3th
export smfmisc-mk := 
#smfmisc-mk += zlib/

export smfnet-mk := 
# smfnet-mk += live555/

export smfaudiocodec-mk :=

export smfaudioprocess-mk :=

export smfvideocodec-mk :=
smfvideocodec-mk += vc9000/vc9000le/
#smfvideocodec-mk += vc9000/memalloc/
smfvideocodec-mk += vc9000/vc9000ld/

export smflibs-mk :=
smflibs-mk += libsmfapi.a
ifneq ($(SMF_OPENSOURCE),1)
smflibs-mk += libsmfappscamera.a
smflibs-mk += libsmfappscameraap.a
smflibs-mk += libsmfappscameraisp.a
endif
smflibs-mk += libsmfappsmmgr.a
smflibs-mk += libsmfappsmsvc.a
smflibs-mk += libsmfappsssvc.a
smflibs-mk += libsmfcommon.a
smflibs-mk += libsmfcore.a
smflibs-mk += libsmfmedia.a
smflibs-mk += libsmfpluginfifo.a
smflibs-mk += libsmfpluginfunc.a
smflibs-mk += libsmfpluginimu.a
smflibs-mk += libsmfpluginshm.a
smflibs-mk += libsmfpluginvpu_vc9000_ld.a
smflibs-mk += libsmfpluginvpu_vc9000_le.a
smflibs-mk += libsmfvideocodecvc9000le.a
smflibs-mk += libsmfvideocodecvc9000ld.a
