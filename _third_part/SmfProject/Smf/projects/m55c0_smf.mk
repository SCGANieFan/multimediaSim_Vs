export path_libs := $(path_root)/libs/$(SMF_PLATFORM)_nuttx/
export smfapps-mk :=
#smfapps-mk += aos/
smfapps-mk += ssvc/
smfapps-mk += msvc/
smfapps-mk += mmgr/
smfapps-mk += camera/
smfapps-mk += camera/lp/
smfapps-mk += camera/isp/
ifeq ($(SMF_RTSP), 1)
smfapps-mk += rtsp4/
# smfapps-mk += rtspclient/
endif

export smfplugin-mk :=
smfplugin-mk += fifo/
smfplugin-mk += wav/
smfplugin-mk += mp3/
# smfplugin-mk += aac/
# smfplugin-mk += opus/
smfplugin-mk += pcm/
smfplugin-mk += func/
smfplugin-mk += device/
#smfplugin-mk += huashan/
smfplugin-mk += shm/
# smfplugin-mk += msvc/
# smfplugin-mk += vad/
smfplugin-mk += mp4/
# smfplugin-mk += kws/
smfplugin-mk += sco/
smfplugin-mk += ipcfs/
smfplugin-mk += h264dem/
smfplugin-mk += time_ctrl/
smfplugin-mk += display/
smfplugin-mk += libs/
smfplugin-mk += dl_v3/
ifeq ($(HTTP_MEDIA_EN),1)
smfplugin-mk += io_http/
endif

export smftest-mk :=
#smftest-mk += bandwidth/

export smfporting-mk :=
smfporting-mk += hal/bes/
smfporting-mk += cpu/m55/
smfporting-mk += fs/nuttx/
smfporting-mk += os/nuttx/
smfporting-mk += osapi/pthread/
smfporting-mk += ipc/openamp_imsvc/
smfporting-mk += dev/codec/
smfporting-mk += algo/other/
smfporting-mk += display/nuttx/
# smfporting-mk += cmd/eshell/
export smfporting-ccflags :=
smfporting-ccflags += -DSMF_USING_CODEC2
smfporting-ccflags += -DSMF_USING_CODEC3

export smfdemo-mk :=
# smfdemo-mk += ssvc/
# smfdemo-mk += algo/
# 
smfdemo-mk += camera/
smfdemo-mk += os/
smfdemo-mk += timer/
smfdemo-mk += mmgr/
# smfdemo-mk += fs/
# smfdemo-mk += apps_aos_api_test/
#smfdemo-mk += usb/
#USB_DEMO += dev
#USB_DEMO += host

# smfdemo-mk += libs/
export smfdemo-libs :=
# smfdemo-libs += pa/
smfdemo-mk += algo/
export ALGO :=
# ALGO += sco/
# ALGO += pa/
# ALGO += normal/
# ALGO += wakeup/
export ALGO_DYN :=
# ALGO_DYN += normal_v3/
# ALGO_DYN += wakeup/
# ALGO_DYN += pa/
# ALGO_DYN += sco/
# ALGO_DYN += vad/
export ALGO_SYNBOL_ADAPTER :=
# ALGO_SYNBOL_ADAPTER += wakeup/
# ALGO_SYNBOL_ADAPTER += pa/
# ALGO_SYNBOL_ADAPTER += vad/
export ALGO_DYNLOAD_THIRD_DLL :=
# ALGO_DYNLOAD_THIRD_DLL += third_algo_ext/
# ALGO_DYNLOAD_THIRD_DLL += pil_algo_normal_dl/
# ALGO_DYNLOAD_THIRD_DLL += vad_algo_ext/
# ALGO_DYNLOAD_THIRD_DLL += wakeup_algo_ext/

# 3th
export smfmisc-mk :=
#smfmisc-mk += zlib/

export smfnet-mk :=
ifeq ($(SMF_RTSP), 1)
smfnet-mk += live555/
endif

export smfaudiocodec-mk :=
#smfaudiocodec-mk += com/
# smfaudiocodec-mk += aac/
# smfaudiocodec-mk += sbc/
# smfaudiocodec-mk += cvsd/
# smfaudiocodec-mk += mp3/
#smfaudiocodec-mk += amr/
#smfaudiocodec-mk += ogg/
#smfaudiocodec-mk += vorbis/
#smfaudiocodec-mk += alac/
#smfaudiocodec-mk += flac/
#smfaudiocodec-mk += opus/
#smfaudiocodec-mk += silk/
#smfaudiocodec-mk += amr/
# export AAC_WATCH := 1

export smfaudioprocess-mk :=
# smfaudioprocess-mk += plc/
# smfaudioprocess-mk += vad/

export smfvideocodec-mk :=
#smfvideocodec-mk += gif/

export smflibs-mk :=
smflibs-mk += libsmfapi.a
smflibs-mk += libsmfappsmsvc.a
smflibs-mk += libsmfappsssvc.a
smflibs-mk += libsmfappsmmgr.a
ifneq ($(SMF_OPENSOURCE),1)
smflibs-mk += libsmfappscamera.a
smflibs-mk += libsmfappscameralp.a
smflibs-mk += libsmfappscameraisp.a
endif
smflibs-mk += libsmfcommon.a
smflibs-mk += libsmfcore.a
smflibs-mk += libsmfmedia.a
smflibs-mk += libsmfplugindevice.a
smflibs-mk += libsmfplugindisplay.a
smflibs-mk += libsmfplugindl_v3.a
smflibs-mk += libsmfpluginfifo.a
smflibs-mk += libsmfpluginfunc.a
smflibs-mk += libsmfpluginh264dem.a
smflibs-mk += libsmfpluginipcfs.a
smflibs-mk += libsmfpluginlibs.a
smflibs-mk += libsmfpluginmp3.a
smflibs-mk += libsmfpluginmp4.a
# smflibs-mk += libsmfpluginopus.a
smflibs-mk += libsmfpluginpcm.a
smflibs-mk += libsmfpluginshm.a
smflibs-mk += libsmfplugintime_ctrl.a
smflibs-mk += libsmfpluginwav.a
smflibs-mk += libsmfpluginsco.a
