#include "sbc_plc_algo_api_c.h"
#include "plc_register_c.h"

plc_api_ret_t sbc_plc_algo_api_com_c::create(plc_api_param_t* plc_api_param, plc_base_port_c* plc_base_port) {

	LOG(plc_base_port->print_cb, "plc api musicplc, (%p,%d,%d,%d),(%d{%d,%d,%d,%d,%d})",
		plc_base_port,
		plc_api_param->fs_hz,
		plc_api_param->channels,
		plc_api_param->frame_samples,
		plc_api_param->param_set,
		plc_api_param->sbc_plc.codec_type,
		plc_api_param->sbc_plc.search_size,
		plc_api_param->sbc_plc.match_size,
		plc_api_param->sbc_plc.olal,
		plc_api_param->sbc_plc.recover);

	if (!plc_base_port
		|| plc_api_param->channels < 1)
		return SBC_PLC_API_RET_FAIL;
	if (plc_api_param->frame_samples < 1
		|| plc_api_param->sbc_plc.codec_type > SBC_PLC_CODEC_TYPE_MAX
		|| plc_api_param->sbc_plc.search_size < 0
		|| plc_api_param->sbc_plc.match_size < 0
		|| plc_api_param->sbc_plc.olal < 0
		|| plc_api_param->sbc_plc.recover < 0)
		return SBC_PLC_API_RET_FAIL;
	_base_porting = plc_base_port;
	i32 ret = init(plc_api_param);
	if(ret != SBC_PLC_API_RET_SUCCESS){
		LOG(plc_base_port->print_cb, "plc api create fail, %d", ret);
		destory();
	}
	return ret;
}

plc_api_ret_t sbc_plc_algo_api_com_c::set(plc_api_set_e choose, void* val) {
	return SBC_PLC_API_RET_SUCCESS;
}

plc_api_ret_t sbc_plc_algo_api_com_c::get(plc_api_get_e choose, void* val) {
	return SBC_PLC_API_RET_SUCCESS;
}

plc_api_ret_t sbc_plc_algo_api_com_c::run(uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, uint16_t is_lost)
{
#if 0
	if (is_lost == false) {
		if (!in
			|| inLen < (frame_samples * _info._bytes_per_sample))
			return SBC_PLC_API_RET_FAIL;
	}
	if (*outLen < frame_samples * _info._bytes_per_sample)
		return SBC_PLC_API_RET_FAIL;
#endif
	run(in, out, is_lost ? true : false);
	if(outLen) *outLen = _byte_per_frame;
	if (inUsed) *inUsed = inLen;
	return SBC_PLC_API_RET_SUCCESS;
}


plc_api_ret_t sbc_plc_algo_api_com_c::destory() {
	deinit();
	_mm.FreeAll();
	return SBC_PLC_API_RET_SUCCESS;
}


i32 sbc_plc_algo_api_com_c::init(plc_api_param_t* plc_api_param) {
	uint16_t width = plc_api_param->data_type == plc_api_data_type_e::PLC_API_DATA_TYPE_SHORT_16 ? 2 : 4;
	_mm.Init(_base_porting);

	//_rate = param->rate;
	_sampleNum = plc_api_param->frame_samples;
	_channels = plc_api_param->channels;
	_codecType = (enum sbc_plc_codec_type_e)plc_api_param->sbc_plc.codec_type;
	_SearchSize = plc_api_param->sbc_plc.search_size;
	_MatchSize = plc_api_param->sbc_plc.match_size;
	_Olal = plc_api_param->sbc_plc.olal;
	_Recover = plc_api_param->sbc_plc.recover;

	struct PLC_State* hd = (struct PLC_State*)_mm.malloc(sizeof(struct PLC_State));
	if (!hd) return SBC_PLC_API_RET_FAIL;
	
	_history[0] = _mm.malloc(sizeof(float) * LHIST_MAX);
	if (!hd) return SBC_PLC_API_RET_FAIL;
	PLC_MEM_SET(_history[0], 0, sizeof(float) * LHIST_MAX);
	
	_rcos = _mm.malloc(sizeof(float) * OLAL_MAX);
	if (!hd) return SBC_PLC_API_RET_FAIL;
	PLC_MEM_SET(_rcos, 0, sizeof(float) * OLAL_MAX);
	
	if (_SearchSize && _MatchSize && _Olal && _Recover)
		a2dp_plc_init(hd, _sampleNum, _SearchSize, _MatchSize, _Olal, _Recover, (float*)_history[0], (float*)_rcos);
	else {
		a2dp_plc_init(hd, _sampleNum, (unsigned int)_codecType, (float*)_history[0], (float*)_rcos);
	}
	_hd[0] = hd;
	if (_channels == 2)
	{
		struct PLC_State* hd1 = (struct PLC_State*)_mm.malloc(sizeof(struct PLC_State));
		if (!hd) return SBC_PLC_API_RET_FAIL;
		_history[1] = _mm.malloc(sizeof(float) * LHIST_MAX);
		if (!_history[1]) return SBC_PLC_API_RET_FAIL;
		memset(_history[0], 0, sizeof(float) * LHIST_MAX);
		if (_SearchSize && _MatchSize && _Olal && _Recover)
		{
			a2dp_plc_init(hd1, _sampleNum, _SearchSize, _MatchSize, _Olal, _Recover, (float*)_history[1], (float*)_rcos);
		}
		else {
			a2dp_plc_init(hd1, _sampleNum, (unsigned int)_codecType, (float*)_history[1], (float*)_rcos);
		}
		_hd[1] = hd1;
	}
	if (!_rcosBuff) {
		_packetNum = _sampleNum * 2;
		_smoothNum = _packetNum * 4;
		_rcosBuff = _mm.malloc(_smoothNum * sizeof(float));
		if (!_rcosBuff) return SBC_PLC_API_RET_FAIL;
	}
	cos_generate((float*)_rcosBuff, _smoothNum, _packetNum);
	_byte_per_frame = _hd[0]->FS*width*_channels;
	_channel_select = plc_api_param->channel_select;
	return SBC_PLC_API_RET_SUCCESS;
}


i32 sbc_plc_algo_api_com_c::deinit() {
	if (_hd[0]) {
		_mm.free(_hd[0]);
		_mm.free(_history[0]);
	}
	if (_hd[1]) {
		_mm.free(_hd[1]);
		_mm.free(_history[1]);
	}
	if (_rcos) {
		_mm.free(_rcos);
		_rcos = NULL;
	}
	if (_rcosBuff) {
		_mm.free(_rcosBuff);
		_rcosBuff = NULL;
	}
	return SBC_PLC_API_RET_SUCCESS;
}

sbc_plc_algo_api_com_c* sbc_plc_algo_api_i16_c::create_hd(plc_base_port_c* plc_base_port) {
	int size = sizeof(sbc_plc_algo_api_i16_c);
	sbc_plc_algo_api_i16_c* plc_api = (sbc_plc_algo_api_i16_c*)plc_base_port->malloc(size);
	if (!plc_api) {
		return 0;
	}
	new(plc_api) sbc_plc_algo_api_i16_c();
	return plc_api;
}

i32 sbc_plc_algo_api_i16_c::run(uint8_t* in, uint8_t* out, b1 is_lost) {
	if (_channels == 2) {
		if (is_lost)
		{
			a2dp_plc_bad_frame((struct PLC_State*)_hd[0], NULL, (short*)out, (float*)_rcosBuff, _packetNum, (float*)_rcos, 2, 0);
			a2dp_plc_bad_frame((struct PLC_State*)_hd[1], NULL, (short*)out, (float*)_rcosBuff, _packetNum, (float*)_rcos, 2, 1);
		}
		else {
			a2dp_plc_good_frame((struct PLC_State*)_hd[0], (short*)in, (short*)out, (float*)_rcosBuff, _packetNum, (float*)_rcos, 2, 0);
			a2dp_plc_good_frame((struct PLC_State*)_hd[1], (short*)in, (short*)out, (float*)_rcosBuff, _packetNum, (float*)_rcos, 2, 1);
		}
	}
	else {
		if (is_lost)
		{
			a2dp_plc_bad_frame((struct PLC_State*)_hd[0], NULL, (short*)out, (float*)_rcosBuff, _packetNum, (float*)_rcos, 1, 0);
		}
		else
		{
			a2dp_plc_good_frame((struct PLC_State*)_hd[0], (short*)in, (short*)out, (float*)_rcosBuff, _packetNum, (float*)_rcos, 1, 0);
		}
	}

	return SBC_PLC_API_RET_FAIL;
}


EXTERNC{

void plc_register_sbc_plc_int16() {
	plc_register_c::Register<sbc_plc_algo_api_i16_c>("sbcplc_i16");
}
#if 0
void plc_register_sbc_plc_int32() {
	plc_register_c::Register<music_plc_algo_api_c<int16_t>>("mplc_i32");
}
void plc_register_sbc_plc_intf32() {
	plc_register_c::Register<music_plc_algo_api_c<int16_t>>("mplc_if32");
}
#endif
}