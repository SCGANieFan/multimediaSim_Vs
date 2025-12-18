#pragma once
#include "gaapi_gaf.h"
#include "music_plc_fill_signal.h"
#include "music_plc_audio_samples.h"
#include "music_plc_overlapadd.h"
#include "music_plc_muter.h"
#include "music_plc_waveform_match.h"

using namespace plc_gaapi_ns;

#define MUSIC_PLC_API_RET_SUCCESS PLC_API_RET_SUCCESS
#define MUSIC_PLC_API_RET_FAIL PLC_API_RET_FAIL

#if 1
enum PlcApiKey_e {
	PLC_API_KEY_DEFAULT = 0,
	PLC_API_KEY_HM_CAST_AP_SLEEP,
	PLC_API_KEY_DJI_DM303,
};

enum plc_api_application_e {
	PLC_API_APPLICATION_VOICE = 0,
	PLC_API_APPLICATION_MUSIC,
	PLC_API_APPLICATION_192K,
	PLC_API_APPLICATION_LOW_LATENCY,
	PLC_API_APPLICATION_LFE,
	PLC_API_APPLICATION_AUTO,
	PLC_API_APPLICATION_MAX,
};
#endif

struct musicplc_param_t {
	int32_t fs_hz;
	int16_t channels;
	int16_t frame_samples;
	uint16_t channel_select;
	const char* param_set;
	int32_t key;
	const char* application;
	int32_t overlap_samples;
	int32_t hold_samples_after_lost;
	int32_t attenuate_samples_after_lost;
	int32_t gain_samples_after_no_lost;
	int32_t seek_samples;
	int32_t no_seek_samples;
	int32_t match_samples;
	bool force_mute_together;
};


template<class T, i8 type = (IsI16<T>() ? 0 : (IsI32<T>() ? 1 : 2))>
struct wave_form_match_plc_c : wave_form_match_instance_c<i16, i16, i64>{};
template<class T>
struct wave_form_match_plc_c<T, 0> : wave_form_match_instance_c<i16, i16, i64, (15 - 4), 0> {};
template<class T>
struct wave_form_match_plc_c<T, 1> : wave_form_match_instance_c<i32, i32, i64, 0, (47 - 4)> {};
template<class T>
struct wave_form_match_plc_c<T, 2> : wave_form_match_instance_c<f32, f32, f32> {};

using PlcApiRet_t = GaapiRet_t;

class music_plc_c : public GaapiGaf_c {
public:
	music_plc_c(muter_c* muter_after_lost,
		muter_c* muter_after_no_lost, 
		overlap_add_c* overlap_add,
		wave_form_match_c* wave_form_match
		) {
		_muter_after_lost = muter_after_lost;
		_muter_after_no_lost = muter_after_no_lost;
		_overlap_add = overlap_add;
		_wave_form_match = wave_form_match;
	}
	virtual ~music_plc_c() {}
public:
	virtual PlcApiRet_t Open()override;
	virtual PlcApiRet_t Set(uint32_t key, void* val)override;
	virtual PlcApiRet_t Get(uint32_t key, void* val)override;
	virtual PlcApiRet_t Receive(GaapiData_c& iData)override;
	virtual PlcApiRet_t Run(GaapiData_c& iData, GaapiData_c& oData)override;
	virtual PlcApiRet_t Generate(GaapiData_c& oData)override;
	virtual PlcApiRet_t Close()override;
private:
	void good_frame(audio_samples_c& p_in, audio_samples_c& p_out);
	void lost_frame(audio_samples_c& p_in, audio_samples_c& p_out);
	i32 run(audio_samples_c& p_in, audio_samples_c& p_out, b1 is_lost);
	void set_param(musicplc_param_t* param);
protected:
	plc_memory_manger_c _mm;
	audio_info_c _info;
	i32 _frame_samples;
	i32 _overlap_samples;
	i32 _hold_samples_after_lost;
	i32 _seek_samples;
	i32 _no_seek_samples;
	i32 _match_samples;
	audio_samples_c _in_history;
	audio_samples_c _in_future;
	music_plc_fill_signal_c _fill_signal;
	muter_c *_muter_after_lost;
	muter_c *_muter_after_no_lost;
	overlap_add_c *_overlap_add;
	wave_form_match_c *_wave_form_match;
	i32 _lost_count;
	i32 _hold_after_lost_samples_now;
	musicplc_param_t* _plc_api_param;
	u16 _channel_select;
	uint16_t _width;
	b1 _is_quick_deal;
	b1 _is_generate_update;
};

template<class T>
class music_plc_algo_api_instance_c : public music_plc_c {
public:
	music_plc_algo_api_instance_c() :
		music_plc_c(&_muter_after_lost,&_muter_after_no_lost,&_overlap_add,&_wave_form_match){
		_width = sizeof(T);
	}
	~music_plc_algo_api_instance_c() {}
private:
	muter_instance_c<T> _muter_after_lost;
	muter_instance_c<T> _muter_after_no_lost;
	overlap_add_instance_c<T> _overlap_add;
	wave_form_match_plc_c<T> _wave_form_match;
};
