#include "plc_api.h"
#include "music_plc.h"

#define LOG_PLC(fmt,...)		   LOG_GAAPI(fmt,##__VA_ARGS__)
class music_plc_algo_api_param_c {
public:
	music_plc_algo_api_param_c() {}
	~music_plc_algo_api_param_c() {}
public:
	void set_plc_param(int32_t ov, int32_t ho, int32_t at, int32_t ga, int32_t se, int32_t ns, int32_t ma) {
		_overlap_ms = ov; _hold_ms_after_lost = ho; _attenuate_ms_after_lost = at; _gain_ms_after_no_lost = ga; _seek_ms = se; _no_seek_ms = ns; _match_ms = ma;
	}
	void set_param_fac(int32_t f0, int32_t f1, int32_t f2, int32_t f3, int32_t f4, int32_t f5, int32_t f6) {
		_fac[0] = f0; _fac[1] = f1; _fac[2] = f2; _fac[3] = f3; _fac[4] = f4; _fac[5] = f5; _fac[6] = f6;
	}
	void convert_to_music_plc_param(musicplc_param_t* param) {
		param->overlap_samples = _overlap_ms * _fac[0];
		param->hold_samples_after_lost = _hold_ms_after_lost * _fac[1];
		param->attenuate_samples_after_lost = _attenuate_ms_after_lost * _fac[2];
		param->gain_samples_after_no_lost = _gain_ms_after_no_lost * _fac[3];
		param->seek_samples = _seek_ms * _fac[4];
		param->no_seek_samples = _no_seek_ms * _fac[5];
		param->match_samples = _match_ms * _fac[6];
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

PlcApiRet_t music_plc_c::Open() {
	set_param(_plc_api_param);
	LOG_PLC("plc api musicplc, (%d,%d,%d),(%d{%d,%d}{%d,%d,%d,%d,%d,%d,%d})",
		_plc_api_param->fs_hz, _plc_api_param->channels, _plc_api_param->frame_samples,
		_plc_api_param->param_set,
		_plc_api_param->key, _plc_api_param->application,
		_plc_api_param->overlap_samples,
		_plc_api_param->hold_samples_after_lost,
		_plc_api_param->attenuate_samples_after_lost,
		_plc_api_param->gain_samples_after_no_lost,
		_plc_api_param->seek_samples,
		_plc_api_param->no_seek_samples,
		_plc_api_param->match_samples);
	if (_plc_api_param->channels < 1)
		return MUSIC_PLC_API_RET_FAIL;
	if (_plc_api_param->frame_samples < 1
		|| _plc_api_param->overlap_samples < 0
		|| _plc_api_param->hold_samples_after_lost < 0
		|| _plc_api_param->attenuate_samples_after_lost < 0
		|| _plc_api_param->gain_samples_after_no_lost < 0
		|| _plc_api_param->seek_samples < 0
		|| _plc_api_param->match_samples < 0)
		return MUSIC_PLC_API_RET_FAIL;

	_mm.Init(&_bp);

	_info.init(_plc_api_param->fs_hz, _width, _plc_api_param->channels);
	_frame_samples = _plc_api_param->frame_samples;
	_overlap_samples = _plc_api_param->overlap_samples;
	_seek_samples = _plc_api_param->seek_samples;
	_no_seek_samples = _plc_api_param->no_seek_samples;
	_match_samples = _plc_api_param->match_samples;
	buffer_samples_c buffer_samples;
	i32 hist_samples = _seek_samples + _match_samples + _no_seek_samples;
	hist_samples = MAX(hist_samples, _frame_samples);
	buffer_samples._samples = hist_samples + _overlap_samples;
	buffer_samples._buf = (u8*)_mm.Malloc(buffer_samples._samples * _info._bytes_per_sample);
	if (!buffer_samples._buf) {
		return MUSIC_PLC_API_RET_FAIL;
	}
	PLC_MEM_SET(buffer_samples._buf, 0, buffer_samples._samples * _info._bytes_per_sample);
	_in_history.init(&_info, &buffer_samples);
#if 0
	for (i16 ch = 0; ch < _info._channels; ch++)
		_in_history.append(_in_history.get_samples_max(), ch);
#else
	_in_history.append(_in_history.get_samples_max(), 0);
#endif
	buffer_samples._samples = _frame_samples;
	buffer_samples._buf = (u8*)_mm.Malloc(buffer_samples._samples * _info._bytes_per_sample);
	if (!buffer_samples._buf) {
		return MUSIC_PLC_API_RET_FAIL;
	}
	PLC_MEM_SET(buffer_samples._buf, 0, buffer_samples._samples * _info._bytes_per_sample);
	_in_future.init(&_info, &buffer_samples);
	_fill_signal.init(&_mm, _in_history.get_samples_max() - _overlap_samples, &_info);
	_muter_after_lost->init(
		&_mm, &_info,
		muter_window_choose_e::MUTER_WINDOW_CHOOSE_COSINE, muter_dir_choose_e::MUTER_DIR_CHOOSE_ATTENUATION,
		_plc_api_param->attenuate_samples_after_lost);
	
	if (_plc_api_param->force_mute_together) {
		_muter_after_no_lost->init(_muter_after_lost, muter_dir_choose_e::MUTER_DIR_CHOOSE_AMPLIFICATION);
	}
	else {
		_muter_after_no_lost->init(
			&_mm, &_info,
			muter_window_choose_e::MUTER_WINDOW_CHOOSE_COSINE, muter_dir_choose_e::MUTER_DIR_CHOOSE_AMPLIFICATION,
			_plc_api_param->gain_samples_after_no_lost);
	}

	_overlap_add->init(&_mm, &_info, overlap_add_window_choose_e::Cosine, _overlap_samples);
	_hold_samples_after_lost = _plc_api_param->hold_samples_after_lost;
	_wave_form_match->Init(wave_form_match_func_mode_e::WAVE_FORM_MATCH_FUNC_MODE_ACCORELATION, &_info);
	// _channel_select = 0xffff;
	_channel_select = _plc_api_param->channel_select;
	_lost_count = 0;
	_is_quick_deal = 0;
	_hold_after_lost_samples_now = 0;
	return MUSIC_PLC_API_RET_SUCCESS;
}

PlcApiRet_t music_plc_c::Set(uint32_t key, void* val) {
	switch (key)
	{
	case Str2Key("param"): _plc_api_param = (musicplc_param_t*)val; return MUSIC_PLC_API_RET_SUCCESS;
	default:
		break;
	}
	return MUSIC_PLC_API_RET_SUCCESS;
}

PlcApiRet_t music_plc_c::Get(uint32_t key, void* val) {
	switch (key)
	{
	case Str2Key("recByte"):
		*(uint32_t*)val = _in_history.get_size_max(); return MUSIC_PLC_API_RET_SUCCESS;
	case Str2Key("ovByte"):
		*(uint32_t*)val = _overlap_samples * _info._bytes_per_sample; return MUSIC_PLC_API_RET_SUCCESS;
	default:
		break;
	}
	return PLC_API_RET_FAIL;
}

PlcApiRet_t music_plc_c::Receive(GaapiData_c& iData) {
	if (iData.Size() != _in_history.get_size_max()) {
		return MUSIC_PLC_API_RET_FAIL;
	}
	_in_history.clear(_in_history.get_valid_samples());
	PLC_MEM_CPY(_in_history.get_left_data(), iData.Data(), iData.Size());
	_in_history.append(_in_history.get_samples_max());
	_is_generate_update = true;
	return MUSIC_PLC_API_RET_SUCCESS;
}

PlcApiRet_t music_plc_c::Run(GaapiData_c& iData, GaapiData_c& oData) {
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
	buffer_samples._buf = (u8*)iData.Data();
	buffer_samples._samples = _frame_samples;
	audio_samples_c p_in;
	p_in.init(&_info, &buffer_samples);

	buffer_samples._buf = (u8*)oData.LeftData();
	buffer_samples._samples = _frame_samples;
	audio_samples_c p_out;
	p_out.init(&_info, &buffer_samples);
	bool is_lost = iData.Flag();
	run(p_in, p_out, is_lost ? true : false);
	oData.Append(p_out.get_valid_samples(0) * _info._bytes_per_sample);
	iData.Used(iData.Size());
	return MUSIC_PLC_API_RET_SUCCESS;
}

PlcApiRet_t music_plc_c::Generate(GaapiData_c& oData) {
	if (_is_generate_update) {
		i32 matchPos = MAX(_match_samples, _overlap_samples);
		_hold_after_lost_samples_now = 0;
		i32 bestLag = _wave_form_match->do_wave_form_match_all_ch(
			_in_history.get_buf_in_sample(0),
			_in_history.get_buf_in_sample(_in_history.get_samples_max() - matchPos),
			_seek_samples,
			_match_samples);
		//overlap add
		_overlap_add->start();
		_overlap_add->do_overlap_add(
			_in_history.get_buf_in_sample(_in_history.get_samples_max() - _overlap_samples),
			_in_history.get_buf_in_sample(_in_history.get_samples_max() - _overlap_samples),
			_in_history.get_buf_in_sample(bestLag + matchPos - _overlap_samples),
			_overlap_samples);
		//fill signal
		_fill_signal.input(
			_in_history,
			bestLag + matchPos,
			_in_history.get_samples_max() - (bestLag + matchPos));
		_muter_after_lost->reset(muter_dir_choose_e::MUTER_DIR_CHOOSE_ATTENUATION);
		_is_generate_update = false;
	}
	//oData
	buffer_samples_c buffer_samples;
	buffer_samples._buf = (u8*)oData.Data();
	buffer_samples._samples = oData.LeftSize() / _info._bytes_per_sample;
	audio_samples_c oDataPlc;
	oDataPlc.init(&_info, &buffer_samples);
	_fill_signal.output(oDataPlc, oDataPlc.get_samples_max());

	//muting
	i32 holdSamples = _hold_samples_after_lost - _hold_after_lost_samples_now;
	holdSamples = MIN(holdSamples, _frame_samples);
	_hold_after_lost_samples_now += holdSamples;
	i32 doMuteSamples = oDataPlc.get_valid_samples() - holdSamples;
	_muter_after_lost->doMute(oDataPlc.get_buf_in_sample(holdSamples), doMuteSamples);
	oData.Append(oDataPlc.get_valid_samples(0) * _info._bytes_per_sample);
	return MUSIC_PLC_API_RET_SUCCESS;
}


PlcApiRet_t music_plc_c::Close() {
	_mm.FreeAll();
	return MUSIC_PLC_API_RET_SUCCESS;
}


void music_plc_c::good_frame(audio_samples_c& p_in, audio_samples_c& p_out)
{
	if (_lost_count > 0) {
		_overlap_add->start();
		_lost_count = 0;
		_muter_after_no_lost->sync(_muter_after_lost);
	}

	if (_muter_after_no_lost->is_mute_finish()
		&& _overlap_add->is_finish()) {
		_is_quick_deal = true;
		return;
	}

	//infuture
	if (!_overlap_add->is_finish()) {
		_fill_signal.output(_in_future, _frame_samples);
#if 1
		_overlap_add->do_overlap_add(
			_in_future.get_buf_in_sample(0),
			_in_future.get_buf_in_sample(0),
			p_in.get_buf_in_sample(0),
			_frame_samples);
#endif
	}
	else
		_in_future.append(p_in, 0, _frame_samples);
}

void music_plc_c::lost_frame(audio_samples_c& p_in, audio_samples_c& p_out)
{
	_lost_count += 1;
	if (_lost_count == 1)
	{
		i32 matchPos = MAX(_match_samples, _overlap_samples);
		_hold_after_lost_samples_now = 0;
#if 1
		i32 bestLag = _wave_form_match->do_wave_form_match_all_ch(
			_in_history.get_buf_in_sample(0),
			_in_history.get_buf_in_sample(_in_history.get_samples_max() - matchPos),
			_seek_samples,
			_match_samples);
#else		
		i32 bestLag = 0;
#endif
		//overlap add
#if 1
		_overlap_add->start();
		_overlap_add->do_overlap_add(
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
		_muter_after_lost->sync(_muter_after_no_lost);
	}
	//_in_future
	_fill_signal.output(_in_future, _in_future.get_samples_max());
}


i32 music_plc_c::run(audio_samples_c& p_in, audio_samples_c& p_out, b1 is_lost) {
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
		_muter_after_no_lost->doMute(_in_future.get_buf_in_sample(0), _frame_samples);
	}
	else {
		i32 holdSamples = _hold_samples_after_lost - _hold_after_lost_samples_now;
		holdSamples = MIN(holdSamples, _frame_samples);
		i32 doMuteSamples = _frame_samples - holdSamples;
		_muter_after_lost->doMute(_in_future.get_buf_in_sample(holdSamples), doMuteSamples);
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


void music_plc_c::set_param(musicplc_param_t* param) {
	uint32_t paramSetKey = Str2Key(_plc_api_param->param_set);
	//uint32_t appKey = Str2Key(_plc_api_param->application);
	uint32_t appKey = Str2Key(_plc_api_param->application);
	if (paramSetKey == Str2Key("param")) {
		param->overlap_samples = _plc_api_param->overlap_samples;
		param->hold_samples_after_lost = _plc_api_param->hold_samples_after_lost;
		param->attenuate_samples_after_lost = _plc_api_param->attenuate_samples_after_lost;
		param->gain_samples_after_no_lost = _plc_api_param->gain_samples_after_no_lost;
		param->seek_samples = _plc_api_param->seek_samples;
		param->no_seek_samples = _plc_api_param->no_seek_samples;
		param->match_samples = _plc_api_param->match_samples;
	}
	else if (paramSetKey == Str2Key("keyapp")) {
		int32_t samples_per_ms = _plc_api_param->fs_hz / 1000;
		music_plc_algo_api_param_c default_param;
		//_overlap_ms _hold_ms_after_lost _attenuate_ms_after_lost _gain_ms_after_no_lost _seek_ms _no_seek_ms _match_ms 
		default_param.set_plc_param(2, 0, 30, 30, 10, 10, 2);
		default_param.set_param_fac(samples_per_ms, samples_per_ms, samples_per_ms, samples_per_ms, samples_per_ms, samples_per_ms, samples_per_ms);
		if (_plc_api_param->key == PLC_API_KEY_DEFAULT) {
			switch (appKey) {
			case Str2Key("auto"):
				default_param.set_plc_param(2, 0, 30, 30, 10, 10, 2);
				break;
			case Str2Key("voice"):
				default_param.set_plc_param(3, 0, 60, 60, 10, 10, 4);
				break;
			case Str2Key("music"):
				default_param.set_plc_param(2, 0, 30, 30, 10, 10, 2);
				break;
			default:
				break;
			}
		}
		else if (_plc_api_param->key == PLC_API_KEY_HM_CAST_AP_SLEEP) {
			switch (appKey) {
			case Str2Key("music"):
				default_param.set_plc_param(2, 0, 50, 50, 10, 10, 2);
				break;
			case Str2Key("app192k"):
				default_param.set_plc_param(2, 0, 30, 30, 90, 5, 100);
				default_param.set_param_fac(samples_per_ms, samples_per_ms, samples_per_ms, samples_per_ms, 1, samples_per_ms, 1);
				break;
			default:
				break;
			}
		}
		else if (_plc_api_param->key == PLC_API_KEY_DJI_DM303) {
			switch (appKey) {
			case Str2Key("lowLate"):
				default_param.set_plc_param(1, 0, 20, 20, 10, 10, 2);
				break;
			default:
				break;
			}
		}
		default_param.convert_to_music_plc_param(_plc_api_param);
	}
}


EXTERNC{

void plc_register_music_plc_int16() {
	gaapi_gaf_register<music_plc_algo_api_instance_c<int16_t>>("mplci16");
}
void plc_register_music_plc_int32() {
	gaapi_gaf_register<music_plc_algo_api_instance_c<int32_t>>("mplci32");
}
void plc_register_music_plc_intf32() {
	gaapi_gaf_register<music_plc_algo_api_instance_c<f32>>("mplcf32");
}

}