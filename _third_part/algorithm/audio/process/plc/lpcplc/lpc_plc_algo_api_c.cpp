#include "lpc_plc_algo_api_c.h"
#include "plc_register_c.h"

plc_api_ret_t lpc_plc_algo_api_com_c::create(plc_api_param_t* plc_api_param, plc_base_port_c* plc_base_port) {

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

plc_api_ret_t lpc_plc_algo_api_com_c::set(plc_api_set_e choose, void* val) {
	return SBC_PLC_API_RET_SUCCESS;
}

plc_api_ret_t lpc_plc_algo_api_com_c::get(plc_api_get_e choose, void* val) {
	return SBC_PLC_API_RET_SUCCESS;
}

plc_api_ret_t lpc_plc_algo_api_com_c::run(uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, uint16_t is_lost)
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


plc_api_ret_t lpc_plc_algo_api_com_c::destory() {
	deinit();
	_mm.FreeAll();
	return SBC_PLC_API_RET_SUCCESS;
}


i32 lpc_plc_algo_api_com_c::init(plc_api_param_t* plc_api_param) {
	uint16_t width = plc_api_param->data_type == plc_api_data_type_e::PLC_API_DATA_TYPE_SHORT_16 ? 2 : 4;
	_mm.Init(_base_porting);
	return SBC_PLC_API_RET_SUCCESS;
}


i32 lpc_plc_algo_api_com_c::deinit() {
	return SBC_PLC_API_RET_SUCCESS;
}

lpc_plc_algo_api_com_c* lpc_plc_algo_api_i16_c::create_hd(plc_base_port_c* plc_base_port) {
	int size = sizeof(lpc_plc_algo_api_i16_c);
	lpc_plc_algo_api_i16_c* plc_api = (lpc_plc_algo_api_i16_c*)plc_base_port->malloc(size);
	if (!plc_api) {
		return 0;
	}
	new(plc_api) lpc_plc_algo_api_i16_c();
	return plc_api;
}

i32 lpc_plc_algo_api_i16_c::run(uint8_t* in, uint8_t* out, b1 is_lost) {
	return SBC_PLC_API_RET_FAIL;
}


EXTERNC{

void plc_register_lpc_plc_int16() {
	plc_register_c::Register<lpc_plc_algo_api_i16_c>("sbcplc_i16");
}
}