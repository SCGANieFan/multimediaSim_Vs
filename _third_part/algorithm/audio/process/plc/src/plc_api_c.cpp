
#include <new>
#include "plc_api_c.h"
#include "plc_register_c.h"
#include "plc_api_base_port_c.h"
#include "plc_base.h"


plc_api_ret_t plc_api_c::create(void** pHd, plc_api_param_t* param) {
	if (!pHd
		|| !param
		|| !param->cb_printf)
		return PLC_API_RET_INPUT_ERROR;
	LOG(param->cb_printf, "plc api (%s) (%p,%p) (%d,%d)",
		_version,
		pHd, param, 
		param->mode, param->data_type);
	int32_t size = sizeof(plc_api_c);
	plc_api_c* plcApi = (plc_api_c*)param->cb_malloc(size);
	if (!plcApi) {
		LOG(param->cb_printf, "plc api malloc fail, (%p,%d)", param->cb_malloc, size);
		return PLC_API_RET_FAIL;
	}
	new(plcApi) plc_api_c();
	plc_api_ret_t ret = plcApi->init(param);
	if (ret != PLC_API_RET_SUCCESS) {
		destory(plcApi);
		return ret;
	}
	*pHd = plcApi;
	LOG(param->cb_printf, "plc api create success, (%p,%p)", plcApi, plcApi->_plc_algo);
	return PLC_API_RET_SUCCESS;
}

plc_api_ret_t plc_api_c::run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost) {
	if (!hd)
		return PLC_API_RET_INPUT_ERROR;
	if (!isLost){
		if (!in
			|| inLen < 1)
			return PLC_API_RET_INPUT_ERROR;
	}
	if (!out)
		return PLC_API_RET_INPUT_ERROR;
	plc_api_c *plcApi = (plc_api_c*)hd;
	return plcApi->_plc_algo_api->run(plcApi->_plc_algo, in, inLen, inUsed, out, pOutLen, isLost);
}
plc_api_ret_t plc_api_c::set(void* hd, plc_api_set_e choose, void* val) {
	if (!hd) {
		return PLC_API_RET_INPUT_ERROR;
	}
	plc_api_c* plcApi = (plc_api_c*)hd;
	LOG(plcApi->_api_base_port.print_cb, "plc api set, (%p,%d,%d)", hd, choose, val);
	return plcApi->_plc_algo_api->set(plcApi->_plc_algo, choose, val);
}
plc_api_ret_t plc_api_c::get(void* hd, plc_api_get_e choose, void* val) {
	if (!hd) {
		return PLC_API_RET_INPUT_ERROR;
	}
	plc_api_c* plcApi = (plc_api_c*)hd;
	LOG(plcApi->_api_base_port.print_cb, "plc api get, (%p,%d,%d)", hd, choose, val);
	return plcApi->_plc_algo_api->get(plcApi->_plc_algo, choose, val);
}
plc_api_ret_t plc_api_c::destory(void* hd) {
	if (!hd)
		return PLC_API_RET_INPUT_ERROR;
	plc_api_c* plcApi = (plc_api_c*)hd;
	LOG(plcApi->_api_base_port.print_cb, "plc api destory");
	plc_api_base_port_c basePort = plcApi->_api_base_port;
	if(plcApi->_plc_algo_api){
		if (plcApi->_plc_algo) {
			plcApi->_plc_algo_api->destory(plcApi->_plc_algo);
			plcApi->_plc_algo = 0;
		}
		plcApi->_plc_algo_api->~plc_algo_api_c();
		basePort.free(plcApi->_plc_algo_api);
		plcApi->_plc_algo_api = 0;
	}
	plcApi->~plc_api_c();
	basePort.free(hd);
	return PLC_API_RET_SUCCESS;
}


plc_api_ret_t plc_api_c::init(plc_api_param_t* param) {
	_api_base_port.malloc_cb = param->cb_malloc;
	_api_base_port.free_cb = param->cb_free;
	_api_base_port.print_cb = param->cb_printf;
	plc_register_c::Print(&_api_base_port);

	switch (param->mode) {
	case plc_api_mode_e::PLC_API_MODE_MUSIC_PLC:
		if (param->data_type == plc_api_data_type_e::PLC_API_DATA_TYPE_SHORT_16) {
			_plc_algo_api = (plc_algo_api_c*)plc_register_c::Create("mplc_i16", &_api_base_port);
		}
		else if (param->data_type == plc_api_data_type_e::PLC_API_DATA_TYPE_INT_32) {
			_plc_algo_api = (plc_algo_api_c*)plc_register_c::Create("mplc_i32", &_api_base_port);
		}
		else if (param->data_type == plc_api_data_type_e::PLC_API_DATA_TYPE_FLOAT_32) {
			_plc_algo_api = (plc_algo_api_c*)plc_register_c::Create("mplc_if32", &_api_base_port);
		}
		else {
			return PLC_API_RET_FAIL;
		}
		break;
	case plc_api_mode_e::PLC_API_MODE_SBC_PLC:
		if (param->data_type == plc_api_data_type_e::PLC_API_DATA_TYPE_SHORT_16) {
			_plc_algo_api = (plc_algo_api_c*)plc_register_c::Create("sbcplc_i16", &_api_base_port);
		}
		else {
			return PLC_API_RET_FAIL;
		}
		break;
	case plc_api_mode_e::PLC_API_MODE_TS_PLC:
		if (param->data_type == plc_api_data_type_e::PLC_API_DATA_TYPE_SHORT_16) {
			_plc_algo_api = (plc_algo_api_c*)plc_register_c::Create("tsplc_i16", &_api_base_port);
		}
		else {
			return PLC_API_RET_FAIL;
		}
		break;
	default:
		LOG(_api_base_port.print_cb, "plc api mode error, %d", param->mode);
		return PLC_API_RET_FAIL;
		break;
	}
	if (!_plc_algo_api) {
		LOG(param->cb_printf, "plc api create fail, %d", PLC_API_RET_NOT_SUPPORT);
		return PLC_API_RET_NOT_SUPPORT;
	}
	plc_api_ret_t ret = _plc_algo_api->create(&_plc_algo, param, &_api_base_port);
	if (ret != PLC_API_RET_SUCCESS) {
		LOG(param->cb_printf, "plc api create fail, %d", ret);
	}
	return ret;
}

