#include "lpc_plc_algo_api_c.h"
#include "plc_register_c.h"

plc_api_ret_t lpc_plc_algo_api_com_c::create(void** pHd, plc_api_param_t* plc_api_param, plc_base_port_c* plc_base_port) {

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
	lpc_plc_algo_api_com_c* plc_api = create_hd(plc_base_port);
	if (!plc_api) {
		return SBC_PLC_API_RET_FAIL;
	}
	plc_api->_base_porting = plc_base_port;
	i32 ret = plc_api->init(plc_api_param);
	if(ret != SBC_PLC_API_RET_SUCCESS){
		LOG(plc_base_port->print_cb, "plc api create fail, %d", ret);
		destory(plc_api);
		plc_api = 0;
	}
	*pHd = plc_api;
	return ret;
}

plc_api_ret_t lpc_plc_algo_api_com_c::set(void* hd, plc_api_set_e choose, void* val) {
	return SBC_PLC_API_RET_SUCCESS;
}

plc_api_ret_t lpc_plc_algo_api_com_c::get(void* hd, plc_api_get_e choose, void* val) {
	return SBC_PLC_API_RET_SUCCESS;
}

plc_api_ret_t lpc_plc_algo_api_com_c::run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, uint16_t is_lost)
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
	lpc_plc_algo_api_com_c* plc = (lpc_plc_algo_api_com_c*)hd;
	plc->run(in, out, is_lost ? true : false);
	if(outLen) *outLen = plc->_byte_per_frame;
	if (inUsed) *inUsed = inLen;
	return SBC_PLC_API_RET_SUCCESS;
}


plc_api_ret_t lpc_plc_algo_api_com_c::destory(void* hd) {
	lpc_plc_algo_api_com_c* plc = (lpc_plc_algo_api_com_c*)hd;
	if (!plc) return SBC_PLC_API_RET_SUCCESS;
	plc->deinit();
	plc->_mm.FreeAll();
	plc_base_port_c* base_porting = plc->_base_porting;
	plc->~lpc_plc_algo_api_com_c();
	base_porting->free(plc);
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