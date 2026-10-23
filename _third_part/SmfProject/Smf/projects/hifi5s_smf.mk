export path_libs := $(path_root)/libs/$(SMF_PLATFORM)/
export smfapps-mk :=
smfapps-mk += ssvc/
smfapps-mk += msvc/
smfapps-mk += camera/isp/

export smfplugin-mk :=
# smfplugin-mk += button/
smfplugin-mk += plc/
smfplugin-mk += cvsd/
smfplugin-mk += mp3/
smfplugin-mk += sbc/
smfplugin-mk += aac/
# smfplugin-mk += opus/
smfplugin-mk += pcm/
smfplugin-mk += device/
smfplugin-mk += fifo/
smfplugin-mk += shm/
smfplugin-mk += func/
smfplugin-mk += h264/
smfplugin-mk += pixel/
smfplugin-mk += resamples/
smfplugin-mk += a2dp_watch/
smfplugin-mk += libs/
# smfplugin-mk += ipcfs/
smfplugin-mk += time_ctrl/
smfplugin-mk += display/
#smfplugin-mk += speech_algo/
# smfplugin-mk += call_record/
# smfplugin-mk += sco_intersys/
ifneq ($(MULTIMEDIA_DISABLE), 1)
ifeq ($(MM_AUDIO_PROCESS_ENABLED),1)
smfplugin-mk += audio_process_watch/
endif
endif
# smfplugin-mk += dl_v3/

export smftest-mk :=
#smftest-mk += bandwidth/

export smfporting-mk :=
smfporting-mk += hal/bes/
smfporting-mk += cpu/hifi5s/
smfporting-mk += fs/other/
smfporting-mk += os/freertos/
smfporting-mk += osapi/cmsis_v2/
smfporting-mk += dev/other/
smfporting-mk += algo/other/
smfporting-mk += display/other/
smfporting-mk += ipc/openamp_imsvc/
smfporting-mk += platform/bes1700/
export smfporting-ccflags :=
smfporting-ccflags += -DSMF_USING_CODEC2
smfporting-ccflags += -DSMF_USING_CODEC3

export smfdemo-mk :=
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
# export ALGO_DYNLOAD_THIRD_DLL :=
# ALGO_DYNLOAD_THIRD_DLL += normal_algo_ext/
# ALGO_DYNLOAD_THIRD_DLL += third_algo_ext/
# ALGO_DYNLOAD_THIRD_DLL += pil_algo_normal_dl/
# ALGO_DYNLOAD_THIRD_DLL += vad_algo_ext/
# ALGO_DYNLOAD_THIRD_DLL += wakeup_algo_ext/

# 3th
export smfmisc-mk :=
#smfmisc-mk += zlib/

export smfnet-mk :=
#smfnet-mk += live555/

export smfaudiocodec-mk :=
smfaudiocodec-mk += aac/
smfaudiocodec-mk += sbc/
smfaudiocodec-mk += cvsd/
smfaudiocodec-mk += mp3/
# smfaudiocodec-mk += opus/
# export OPUS := opt/1.3.1
#export OPUS := fd/1.2.1

export smfaudioprocess-mk :=
smfaudioprocess-mk += plc/
#smfaudioprocess-mk += resamples/

export smfvideocodec-mk :=
smfvideocodec-mk += h264Hifi/
#smfvideocodec-mk += libde265/
#smfvideocodec-mk += gif/

export smflibs-mk :=
smflibs-mk += libsmfapi.a
smflibs-mk += libsmfappsmsvc.a
smflibs-mk += libsmfappsssvc.a
smflibs-mk += libsmfaudiocodecaac.a
smflibs-mk += libsmfaudiocodeccvsd.a
smflibs-mk += libsmfaudiocodecmp3.a
# smflibs-mk += libsmfaudiocodecopus.a
smflibs-mk += libsmfaudiocodecsbc.a
smflibs-mk += libsmfaudioprocessplc.a
smflibs-mk += libsmfcommon.a
smflibs-mk += libsmfcore.a
smflibs-mk += libsmfmedia.a
smflibs-mk += libsmfplugina2dp_watch.a
# smflibs-mk += libsmfpluginaac.a
ifneq ($(MULTIMEDIA_DISABLE), 1)
ifeq ($(MM_AUDIO_PROCESS_ENABLED), 1)
smflibs-mk += libsmfpluginaudio_process_watch.a
endif
endif
# smflibs-mk += libsmfplugincvsd.a
smflibs-mk += libsmfplugindevice.a
# smflibs-mk += libsmfplugindl_v3.a
smflibs-mk += libsmfpluginfifo.a
smflibs-mk += libsmfpluginfunc.a
smflibs-mk += libsmfpluginlibs.a
smflibs-mk += libsmfpluginaac.a
smflibs-mk += libsmfpluginmp3.a
# smflibs-mk += libsmfpluginopus.a
smflibs-mk += libsmfpluginpcm.a
smflibs-mk += libsmfpluginplc.a
smflibs-mk += libsmfpluginresamples.a
smflibs-mk += libsmfpluginsbc.a
smflibs-mk += libsmfplugincvsd.a
smflibs-mk += libsmfpluginshm.a
