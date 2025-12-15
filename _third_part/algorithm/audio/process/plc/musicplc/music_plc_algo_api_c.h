#pragma once
#include<new>
#include "music_plc_fill_signal_c.h"
#include "plc_base.h"
#include "plc_algo_api_c.h"
#include "plc_base_port_c.h"
#include "plc_audio_samples_c.h"
#include "plc_overlapadd_c.h"
#include "plc_audio_cal_muter_c.h"
#include "plc_audio_cal_waveform_match_c.h"


#define MUSIC_PLC_API_RET_SUCCESS PLC_API_RET_SUCCESS
#define MUSIC_PLC_API_RET_FAIL PLC_API_RET_FAIL

enum PlcApiKey_e {
	PLC_API_KEY_DEFAULT = 0,
	PLC_API_KEY_HM_CAST_AP_SLEEP,
	PLC_API_KEY_DJI_DM303,
};

template<class T, i8 type = (IsI16<T>() ? 0 : (IsI32<T>() ? 1 : 2))>
struct wave_form_match_plc_c : wave_form_match_c<i16, i16, u64>{};
template<class T>
struct wave_form_match_plc_c<T, 0> : wave_form_match_c<i16, i16, i64, (15 - 4), 0> {};
template<class T>
struct wave_form_match_plc_c<T, 1> : wave_form_match_c<i32, i32, i64, 0, (47 - 4)> {};
template<class T>
struct wave_form_match_plc_c<T, 2> : wave_form_match_c<f32, f32, f32> {};


template<class T>
class music_plc_algo_api_c : public plc_algo_api_c {
public:
	music_plc_algo_api_c() {}
	virtual ~music_plc_algo_api_c() {}
public:
	plc_api_ret_t create(plc_api_param_t* plc_api_param, plc_base_port_c* plc_base_port) override;
	plc_api_ret_t set(plc_api_set_e choose, void* val) override;
	plc_api_ret_t get(plc_api_get_e choose, void* val) override;
	plc_api_ret_t run(uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t is_lost) override;
	plc_api_ret_t destory() override;
private:
	void good_frame(audio_samples_c& p_in, audio_samples_c& p_out);
	void lost_frame(audio_samples_c& p_in, audio_samples_c& p_out);
	i32 init(plc_api_param_t* plc_api_param);
	i32 run(audio_samples_c& p_in, audio_samples_c& p_out, b1 is_lost);
	void set_param(plc_api_param_t* plc_api_param);

private:
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
	muter_c<T> _muter_after_lost;
	muter_c<T> _muter_after_no_lost;
	overlap_add_c<T> _overlap_add;
	wave_form_match_plc_c<T> _wave_form_match;
	i32 _lost_count;
	i32 _hold_after_lost_samples_now;
	plc_base_port_c* _base_porting;
	u16 _channel_select;
	b1 _is_quick_deal;
};


class music_plc_algo_api_param_c{
public:
	music_plc_algo_api_param_c() {}
	~music_plc_algo_api_param_c() {}
public:
	void set_plc_param(int32_t ov, int32_t ho, int32_t at, int32_t ga, int32_t se, int32_t ns, int32_t ma) {
		_overlap_ms = ov;_hold_ms_after_lost = ho;_attenuate_ms_after_lost = at;_gain_ms_after_no_lost = ga;_seek_ms = se;_no_seek_ms = ns;_match_ms = ma;
	}
	void set_param_fac(int32_t f0, int32_t f1, int32_t f2, int32_t f3, int32_t f4, int32_t f5, int32_t f6) {
		_fac[0] = f0;_fac[1] = f1;_fac[2] = f2;_fac[3] = f3;_fac[4] = f4;_fac[5] = f5;_fac[6] = f6;
	}
	void convert_to_music_plc_param(plc_api_param_t* plc_api_param){
		plc_api_param->music_plc.overlap_samples = _overlap_ms * _fac[0];
		plc_api_param->music_plc.hold_samples_after_lost = _hold_ms_after_lost * _fac[1];
		plc_api_param->music_plc.attenuate_samples_after_lost = _attenuate_ms_after_lost * _fac[2];
		plc_api_param->music_plc.gain_samples_after_no_lost = _gain_ms_after_no_lost * _fac[3];
		plc_api_param->music_plc.seek_samples = _seek_ms * _fac[4];
		plc_api_param->music_plc.no_seek_samples = _no_seek_ms * _fac[5];
		plc_api_param->music_plc.match_samples = _match_ms * _fac[6];
	}
public:
	int32_t _overlap_ms;
	int32_t _hold_ms_after_lost;
	int32_t _attenuate_ms_after_lost;
	int32_t _gain_ms_after_no_lost;
	int32_t _seek_ms;
	int32_t _no_seek_ms;
	int32_t _match_ms;
	int32_t _fac[7];
};

template<class T>
plc_api_ret_t music_plc_algo_api_c<T>::create(plc_api_param_t* plc_api_param, plc_base_port_c* plc_base_port) {
	set_param(plc_api_param);
	LOG(plc_base_port->print_cb, "plc api musicplc, (%p,%d,%d,%d),(%d{%d,%d}{%d,%d,%d,%d,%d,%d,%d})",
		plc_base_port,
		plc_api_param->fs_hz,
		plc_api_param->channels,
		plc_api_param->frame_samples,
		plc_api_param->param_set,
		plc_api_param->key,
		plc_api_param->application,
		plc_api_param->music_plc.overlap_samples,
		plc_api_param->music_plc.hold_samples_after_lost,
		plc_api_param->music_plc.attenuate_samples_after_lost,
		plc_api_param->music_plc.gain_samples_after_no_lost,
		plc_api_param->music_plc.seek_samples,
		plc_api_param->music_plc.no_seek_samples,
		plc_api_param->music_plc.match_samples);
	if (!plc_base_port
		|| plc_api_param->channels < 1)
		return MUSIC_PLC_API_RET_FAIL;
	if (plc_api_param->frame_samples < 1
		|| plc_api_param->music_plc.overlap_samples < 0
		|| plc_api_param->music_plc.hold_samples_after_lost < 0
		|| plc_api_param->music_plc.attenuate_samples_after_lost < 0
		|| plc_api_param->music_plc.gain_samples_after_no_lost < 0
		|| plc_api_param->music_plc.seek_samples < 0
		|| plc_api_param->music_plc.match_samples < 0)
		return MUSIC_PLC_API_RET_FAIL;
	_base_porting = plc_base_port;
	i32 ret = init(plc_api_param);
	if(ret != MUSIC_PLC_API_RET_SUCCESS){
		LOG(plc_base_port->print_cb, "plc api create fail, %d", ret);
	}
	return ret;
}
template<class T>
plc_api_ret_t music_plc_algo_api_c<T>::set(plc_api_set_e choose, void* val) {
#if 0
	if (!hd
		|| choose >= MUSIC_PLC_SET_CHOOSE_MAX)
		return MUSIC_PLC_API_RET_FAIL;
	music_plc_algo_api_c<T>* plc = (music_plc_algo_api_c<T>*)hd;
	switch (choose)
	{
	default:
		break;
	}
#endif
	return MUSIC_PLC_API_RET_SUCCESS;
}
template<class T>
plc_api_ret_t music_plc_algo_api_c<T>::get(plc_api_get_e choose, void* val) {
	return MUSIC_PLC_API_RET_SUCCESS;
}
template<class T>
plc_api_ret_t music_plc_algo_api_c<T>::run(uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, uint16_t is_lost)
{
#if 0
	if (is_lost == false) {
		if (!in
			|| inLen < (frame_samples * _info._bytes_per_sample))
			return MUSIC_PLC_API_RET_FAIL;
	}
	if (*outLen < frame_samples * _info._bytes_per_sample)
		return MUSIC_PLC_API_RET_FAIL;
#endif
	buffer_samples_c buffer_samples;
	buffer_samples._buf = in;
	buffer_samples._samples = _frame_samples;
	audio_samples_c p_in;
	p_in.init(&_info, &buffer_samples);

	buffer_samples._buf = out;
	buffer_samples._samples = _frame_samples;
	audio_samples_c p_out;
	p_out.init(&_info, &buffer_samples);
	run(p_in, p_out, is_lost ? true : false);
	if(outLen) *outLen = p_out.get_valid_samples(0) * _info._bytes_per_sample;
	if (inUsed) *inUsed = inLen;
	return MUSIC_PLC_API_RET_SUCCESS;
}

template<class T>
plc_api_ret_t music_plc_algo_api_c<T>::destory() {
	_mm.FreeAll();
	return MUSIC_PLC_API_RET_SUCCESS;
}


template<class T>
void music_plc_algo_api_c<T>::good_frame(audio_samples_c& p_in, audio_samples_c& p_out)
{
	if (_lost_count > 0) {
		_overlap_add.start();
		_lost_count = 0;
		_muter_after_no_lost.sync(_muter_after_lost);
	}

	if (_muter_after_no_lost.is_mute_finish()
		&& _overlap_add.is_finish()) {
		_is_quick_deal = true;
		return;
	}

	//infuture
	if (!_overlap_add.is_finish()) {
		_fill_signal.output(_in_future, _frame_samples);
#if 1
		_overlap_add.do_overlap_add(
			_in_future.get_buf_in_sample(0),
			_in_future.get_buf_in_sample(0),
			p_in.get_buf_in_sample(0),
			_frame_samples);
#endif
	}
	else
		_in_future.append(p_in, 0, _frame_samples);
}

template<class T>
void music_plc_algo_api_c<T>::lost_frame(audio_samples_c& p_in, audio_samples_c& p_out)
{
	_lost_count += 1;
	if (_lost_count == 1)
	{
		i32 matchPos = MAX(_match_samples, _overlap_samples);
		_hold_after_lost_samples_now = 0;
#if 1
		i32 bestLag = _wave_form_match.do_wave_form_match_all_ch(
			(T*)_in_history.get_buf_in_sample(0),
			(T*)_in_history.get_buf_in_sample(_in_history.get_samples_max() - matchPos),
			_seek_samples,
			_match_samples);
#else		
		i32 bestLag = 0;
#endif
		//overlap add
#if 1
		_overlap_add.start();
		_overlap_add.do_overlap_add(
			_in_history.get_buf_in_sample(_in_history.get_samples_max() - _overlap_samples),
			_in_history.get_buf_in_sample(_in_history.get_samples_max() - _overlap_samples),
			_in_history.get_buf_in_sample(bestLag + matchPos - _overlap_samples),
			_overlap_samples);
#endif
		//fill signal
#if 1
		_fill_signal.input(
			_in_history,
			bestLag + matchPos,
			_in_history.get_samples_max() - (bestLag + matchPos));
#else
		_fill_signal._fill_signal.Clear(_fill_signal._fill_signal.get_valid_samples());
		_fill_signal._fill_signal.Append(_fill_signal._fill_signal.get_samples_max());
		PLC_MEM_SET(_fill_signal._fill_signal.get_buf_in_sample(0), 0, _fill_signal._fill_signal.get_size_max());
		_fill_signal._fill_signal_sample_index[0] = 0;
#endif
		//state updata
		_muter_after_lost.sync(_muter_after_no_lost);
	}
	//_in_future
	_fill_signal.output(_in_future, _in_future.get_samples_max());
}


template<class T>
i32 music_plc_algo_api_c<T>::init(plc_api_param_t* plc_api_param) {
	uint16_t width = plc_api_param->data_type == plc_api_data_type_e::PLC_API_DATA_TYPE_SHORT_16 ? 2 : 4;
	_mm.Init(_base_porting);
	_info.init(plc_api_param->fs_hz, width, plc_api_param->channels);
	_frame_samples = plc_api_param->frame_samples;
	_overlap_samples = plc_api_param->music_plc.overlap_samples;
	_seek_samples = plc_api_param->music_plc.seek_samples;
	_no_seek_samples = plc_api_param->music_plc.no_seek_samples;
	_match_samples = plc_api_param->music_plc.match_samples;
	buffer_samples_c buffer_samples;
#if 1
	i32 hist_samples = _seek_samples + _match_samples + _no_seek_samples;
	hist_samples = MAX(hist_samples, _frame_samples);
	buffer_samples._samples = hist_samples + _overlap_samples;
#endif
	buffer_samples._buf = (u8*)_mm.malloc(buffer_samples._samples * _info._bytes_per_sample);
	if (!buffer_samples._buf) {
		return MUSIC_PLC_API_RET_FAIL;
	}
	PLC_MEM_SET(buffer_samples._buf, 0, buffer_samples._samples * _info._bytes_per_sample);
	_in_history.init(&_info, &buffer_samples);
	_in_history.append(_in_history.get_samples_max(), 0);
	buffer_samples._samples = _frame_samples;
	buffer_samples._buf = (u8*)_mm.malloc(buffer_samples._samples * _info._bytes_per_sample);
	if (!buffer_samples._buf) {
		return MUSIC_PLC_API_RET_FAIL;
	}
	PLC_MEM_SET(buffer_samples._buf, 0, buffer_samples._samples * _info._bytes_per_sample);
	_in_future.init(&_info, &buffer_samples);
	_fill_signal.init(&_mm, _in_history.get_samples_max() - _overlap_samples, &_info);
	_muter_after_lost.init(
		&_mm, &_info,
		muter_window_choose_e::MUTER_WINDOW_CHOOSE_COSINE, muter_dir_choose_e::MUTER_DIR_CHOOSE_ATTENUATION,
		plc_api_param->music_plc.attenuate_samples_after_lost);
	if (plc_api_param->music_plc.force_mute_together) {
		_muter_after_no_lost.init(&_muter_after_lost,muter_dir_choose_e::MUTER_DIR_CHOOSE_AMPLIFICATION);
	}else{
		_muter_after_no_lost.init(
			&_mm, &_info,
			muter_window_choose_e::MUTER_WINDOW_CHOOSE_COSINE, muter_dir_choose_e::MUTER_DIR_CHOOSE_AMPLIFICATION,
			plc_api_param->music_plc.gain_samples_after_no_lost);
	}
	_overlap_add.init(&_mm, &_info, overlap_add_window_choose_e::Cosine, _overlap_samples);
	_hold_samples_after_lost = plc_api_param->music_plc.hold_samples_after_lost;
	_wave_form_match.Init(wave_form_match_func_mode_e::WAVE_FORM_MATCH_FUNC_MODE_ACCORELATION, &_info);
	// _channel_select = 0xffff;
	_channel_select = plc_api_param->channel_select;
	_lost_count = 0;
	_is_quick_deal = 0;
	_hold_after_lost_samples_now = 0;

	return MUSIC_PLC_API_RET_SUCCESS;

}

template<class T>
i32 music_plc_algo_api_c<T>::run(audio_samples_c& p_in, audio_samples_c& p_out, b1 is_lost) {
#if 1
	if (is_lost == false) {
		good_frame(p_in, p_out);
	}
	else {
		lost_frame(p_in, p_out);
	}
	if (_is_quick_deal) {
		_is_quick_deal = false;
		_in_history.clear(_frame_samples);
		_in_history.append(p_in, 0, _frame_samples);
		p_out.append(
			_in_history,
			_in_history.get_samples_max() - _overlap_samples - _frame_samples,
			_frame_samples);
		return MUSIC_PLC_API_RET_SUCCESS;
	}

#if 1
	//muting
	if (is_lost == false) {
		_muter_after_no_lost.doMute(_in_future.get_buf_in_sample(0), _frame_samples);
	}
	else {
		i32 holdSamples = _hold_samples_after_lost - _hold_after_lost_samples_now;
		holdSamples = MIN(holdSamples, _frame_samples);
		i32 doMuteSamples = _frame_samples - holdSamples;
		_muter_after_lost.doMute(_in_future.get_buf_in_sample(holdSamples), doMuteSamples);
		_hold_after_lost_samples_now += holdSamples;
	}
#endif
	//out
	_in_history.clear(_frame_samples);
	_in_history.append(_in_future, 0, _frame_samples);
	p_out.append(
		_in_history,
		_in_history.get_samples_max() - _overlap_samples - _frame_samples,
		_frame_samples);

	_in_future.clear(_in_future.get_valid_samples());
	return MUSIC_PLC_API_RET_SUCCESS;
#endif
}


template<class T>
void music_plc_algo_api_c<T>::set_param(plc_api_param_t* plc_api_param){
	if(plc_api_param->param_set==plc_api_param_set_e::PLC_API_PARAM_SET_PARAM){
#if 0
		param->overlap_samples = plc_api_param->overlap_samples;
		param->hold_samples_after_lost = plc_api_param->hold_samples_after_lost;
		param->attenuate_samples_after_lost = plc_api_param->attenuate_samples_after_lost;
		param->gain_samples_after_no_lost = plc_api_param->gain_samples_after_no_lost;
		param->seek_samples = plc_api_param->seek_samples;
		param->no_seek_samples = plc_api_param->no_seek_samples;
		param->match_samples = plc_api_param->match_samples;
#endif
	}
	else if(plc_api_param->param_set==plc_api_param_set_e::PLC_API_PARAM_SET_KEY_APP){
		int32_t samples_per_ms = plc_api_param->fs_hz / 1000;
		music_plc_algo_api_param_c default_param;
		//_overlap_ms _hold_ms_after_lost _attenuate_ms_after_lost _gain_ms_after_no_lost _seek_ms _no_seek_ms _match_ms 
		default_param.set_plc_param(2, 0, 30, 30, 10, 10, 2);
		default_param.set_param_fac(samples_per_ms, samples_per_ms, samples_per_ms, samples_per_ms, samples_per_ms, samples_per_ms, samples_per_ms);
		if (plc_api_param->key == PLC_API_KEY_DEFAULT) {
			switch (plc_api_param->application) {
			case PLC_API_APPLICATION_AUTO:
				default_param.set_plc_param(2, 0, 30, 30, 10, 10, 2);
				break;
			case PLC_API_APPLICATION_VOICE:
				default_param.set_plc_param(3, 0, 60, 60, 10, 10, 4);
				break;
			case PLC_API_APPLICATION_MUSIC:
				default_param.set_plc_param(2, 0, 30, 30, 10, 10, 2);
				break;
			default:
				break;
			}
		}
		else if (plc_api_param->key == PLC_API_KEY_HM_CAST_AP_SLEEP) {
			switch (plc_api_param->application) {
			case PLC_API_APPLICATION_MUSIC:
				default_param.set_plc_param(2, 0, 50, 50, 10, 10, 2);
				break;
			case PLC_API_APPLICATION_192K:
				default_param.set_plc_param(2,0,30,30,90,5,100);
				default_param.set_param_fac(samples_per_ms, samples_per_ms, samples_per_ms, samples_per_ms, 1, samples_per_ms, 1);
				break;
			default:
				break;
			}
		}
		else if (plc_api_param->key == PLC_API_KEY_DJI_DM303) {
			switch (plc_api_param->application) {
			case PLC_API_APPLICATION_LOW_LATENCY:
				default_param.set_plc_param(1, 0, 20, 20, 10, 10, 2);
				break;
			default:
				break;
			}
		}
		default_param.convert_to_music_plc_param(plc_api_param);
	}
}