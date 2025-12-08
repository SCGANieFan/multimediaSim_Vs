#include "plc_api_private.h"

#define VERSION "2.1.0.0"

plc_api_ret_t plc_api_c::Open() {
	LOG_PLC("plc api %s (%d,%d)", VERSION, _param.mode, _param.data_type);
	uint32_t modeKey = Str2Key(_param.mode);
	uint32_t dataTypeKey = Str2Key(_param.data_type);
	if (modeKey == Str2Key("musicPlc")) {
		if (dataTypeKey == Str2Key("i16"))  _plcAlgo = gaapi_gaf_create("mplci16", &_bp);
		else if (dataTypeKey == Str2Key("i32"))_plcAlgo = gaapi_gaf_create("mplci32", &_bp);
		else if (dataTypeKey == Str2Key("f32"))_plcAlgo = gaapi_gaf_create("mplcf32", &_bp);
	}
	else {
		LOG_PLC("plc api mode error, %d", _param.mode); return PLC_API_RET_FAIL;
	}
	if (!_plcAlgo) {
		LOG_PLC("plc api create fail, %d", PLC_API_RET_NOT_SUPPORT);
		Close();
		return PLC_API_RET_NOT_SUPPORT;
	}
	if (modeKey == Str2Key("musicPlc")) {
		_param.musicPlcParam.fs_hz = _param.fs_hz;
		_param.musicPlcParam.channels = _param.channels;
		_param.musicPlcParam.frame_samples = _param.frame_samples;
		_plcAlgo->Set(Str2Key("param"), (void*)&_param.musicPlcParam);
		_plcAlgo->Open();
	}
	LOG_PLC("plc api create success, (%p)", _plcAlgo);
	return PLC_API_RET_SUCCESS;
}

plc_api_ret_t plc_api_c::Set(const char* choose, void* val) {
	LOG_PLC("(%s, %d)", choose, val);
	uint32_t key = Str2Key(choose);
	switch (key)
	{
	case Str2Key("rate"): _param.fs_hz = (int32_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("ch"): _param.channels = (int16_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("fSample"): _param.frame_samples = (int16_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("dataType"): _param.data_type = (const char*)val; return PLC_API_RET_SUCCESS;
	case Str2Key("mode"): _param.mode = (const char*)val; return PLC_API_RET_SUCCESS;
	//music plc
	case Str2Key("chSelect"): _param.musicPlcParam.channel_select = (uint16_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("paramSet"): _param.musicPlcParam.param_set = (const char*)val; return PLC_API_RET_SUCCESS;
	case Str2Key("key"): _param.musicPlcParam.key = (int32_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("app"): _param.musicPlcParam.application = (const char*)val; return PLC_API_RET_SUCCESS;
	case Str2Key("overlap"): _param.musicPlcParam.overlap_samples = (int32_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("holdAL"): _param.musicPlcParam.hold_samples_after_lost = (int32_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("attAL"): _param.musicPlcParam.attenuate_samples_after_lost = (int32_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("gainANL"): _param.musicPlcParam.gain_samples_after_no_lost = (int32_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("seek"): _param.musicPlcParam.seek_samples = (int32_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("noSeek"): _param.musicPlcParam.no_seek_samples = (int32_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("match"): _param.musicPlcParam.match_samples = (int32_t)(uint32_t)val; return PLC_API_RET_SUCCESS;
	case Str2Key("forceMT"): _param.musicPlcParam.force_mute_together = (bool)(uint32_t)val; return PLC_API_RET_SUCCESS;
	default:
		break;
	}
	if (_plcAlgo) return _plcAlgo->Set(key, val);
	return PLC_API_RET_FAIL;
}

plc_api_ret_t plc_api_c::Get(const char* choose, void* val) {
	LOG_PLC("(%s, %d)", choose, val);
	uint32_t key = Str2Key(choose);
#if 0
	switch (key)
	{
	case Str2Key("test"):return PLC_API_RET_SUCCESS;
	default:
		break;
	}
#endif
	if (_plcAlgo) return _plcAlgo->Get(key, val);
	return PLC_API_RET_FAIL;
}

plc_api_ret_t plc_api_c::Run(GaapiData_c& iData, GaapiData_c& oData) {
	bool isLost = iData.Flag();
	if (!isLost) {
		if (!iData.Data()
			|| iData.Size() < 1)
			return PLC_API_RET_INPUT_ERROR;
	}
	if (!oData.LeftData()
		|| !oData.LeftSize())
		return PLC_API_RET_INPUT_ERROR;
	return _plcAlgo->Run(iData, oData);
}

plc_api_ret_t plc_api_c::Receive(GaapiData_c& iData) {
	return _plcAlgo->Receive(iData);
}

plc_api_ret_t plc_api_c::Generate(GaapiData_c& oData) {
	return _plcAlgo->Generate(oData);
}

plc_api_ret_t plc_api_c::Close() {
	LOG_PLC();
	if (_plcAlgo) {
		_plcAlgo->Close();
		GaapiGaf_c::Destory(_plcAlgo);
		_plcAlgo = 0;
	}
	return PLC_API_RET_SUCCESS;
}


EXTERNC{
uint32_t plc_api_create(plc_api_param_t* param){
	return plc_api_c::CreateApi<plc_api_c>((GaapiBasePort_t*)param);
}

plc_api_ret_t plc_api_open(uint32_t id) {
	return plc_api_c::OpenApi(id);
}

plc_api_ret_t plc_api_receive(uint32_t id, uint8_t* in, int32_t in_len) {
	GaapiData_c iData;
	iData.Init(in, in_len, in_len);
	plc_api_ret_t ret = plc_api_c::ReceiveApi(id, iData);
	return ret;
}

plc_api_ret_t plc_api_run(uint32_t id, uint8_t* in, int32_t in_len, int32_t* in_used, uint8_t* out, int32_t* p_out_len, uint16_t is_lost){
	if (!p_out_len) return PLC_API_RET_INPUT_ERROR;
	GaapiData_c iData;
	GaapiData_c oData;
	iData.Init(in, in_len, in_len);
	iData.Flag(is_lost);
	oData.Init(out, *p_out_len);
	plc_api_ret_t ret = plc_api_c::RunApi(id, iData, oData);
	if (in_used) *in_used = iData.Used();
	*p_out_len = oData.Size();
	return ret;
}

plc_api_ret_t plc_api_generate(uint32_t id, uint8_t* out, int32_t* p_out_len){
	if (!p_out_len) return PLC_API_RET_INPUT_ERROR;
	GaapiData_c oData;
	oData.Init(out, *p_out_len);
	plc_api_ret_t ret = plc_api_c::GenerateApi(id, oData);
	*p_out_len = oData.Size();
	return ret;
}

plc_api_ret_t plc_api_set(uint32_t id, const char *choose, void* val) {
	return plc_api_c::SetApi(id,choose,val);
}

plc_api_ret_t plc_api_get(uint32_t id, const char* choose, void* val) {
	return plc_api_c::GetApi(id,choose,val);
}

plc_api_ret_t plc_api_close(uint32_t id) {
	return plc_api_c::CloseApi(id);
}
plc_api_ret_t plc_api_destory(uint32_t id){
	return plc_api_c::DestoryeApi(id);
}

void plc_api_register_music_plc_int16() {
	extern void plc_register_music_plc_int16();
	plc_register_music_plc_int16();
}

void plc_api_register_music_plc_int32() {
	extern void plc_register_music_plc_int32();
	plc_register_music_plc_int32();
}

void plc_api_register_music_plc_f32() {
	extern void plc_register_music_plc_intf32();
	plc_register_music_plc_intf32();
}
#if 0
void plc_api_register_sbc_plc_i16() {
	extern void plc_register_sbc_plc_int16();
	plc_register_sbc_plc_int16();
}

void plc_api_register_ts_plc_i16() {
	extern void plc_register_ts_plc_int16();
	plc_register_ts_plc_int16();
}
#endif

}
