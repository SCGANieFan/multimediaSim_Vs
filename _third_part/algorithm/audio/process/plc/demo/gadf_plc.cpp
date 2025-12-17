#include <string.h>
#include "gadf_plc.h"
#include "plc_api_demo.h"

bool GadfPlcSourceArray_c::Set(const char* key, void* val) {
	switch (Str2Key(key))
	{
	case Str2Key("sNum"):sim_lost_num = (uint32_t)val; return true;
	case Str2Key("sDen"):sim_lost_den = (uint32_t)val; return true;
	default:
		break;
	}
	return GadfSourceArray_c::Set(key, val);
}

bool GadfPlcSourceArray_c::Generate(GadfData_c& oData) {
	if (!oData.Buf()) {
		oData.Init(_buf, _bufByteMax, _bufByteMax);
	}
	_frameNum++;
	if ((_frameNum - 1) % sim_lost_den < (sim_lost_den - sim_lost_num)) {
		oData.ClearFlag(0x00000001);
	}
	else {
		oData.Flag(0x00000001);
	}
	
	//LOG("%d,%d,%d", _frameNum, oData._size, oData._offset);
	return true;
}

bool GadfPlcSinkArray_c::Receive(GadfData_c& iData) {
	if (!_buf) return false;
	uint32_t iSize = iData.Size();
	if (iSize) {
		if ((iSize + _bufByte) > _bufByteMax) {
			return false;
		}
		uint8_t* pExpected = (uint8_t*)_buf + _bufByte;
		uint8_t* pIn = (uint8_t*)iData.Data();
		for (uint32_t n = 0; n < iSize; n++) {
			if (pIn[n] != pExpected[n]) {
				LOG("test is not success, %d", _bufByte + n);
				return false;
			}
		}
		_bufByte += iSize;
		iData.Used(iSize);
		iData.Clear();
	}
	return true;
}

bool GadfPlcAlgo_c::Init() {
	_frameSample = _f0p1Ms * _rate / 10000;
	_plc_id = plc_api_demo_init(_rate, _channels, _width, _frameSample);
	if (!_plc_id) return false;
	_oBufMax = _frameSample * _width * _channels;
	_oBuf = _bp._malloc(_oBufMax);
	if (!_oBuf) return false;
	return true;
}

bool GadfPlcAlgo_c::Set(const char* key, void* val) {
	switch (Str2Key(key))
	{
	case Str2Key("rate"):_rate = (uint32_t)val; return true;
	case Str2Key("ch"):_channels = (uint16_t)(uint32_t)val; return true;
	case Str2Key("width"):_width = (uint16_t)(uint32_t)val; return true;
	//case Str2Key("fSample"):_frameSample = (uint32_t)val; return true;
	case Str2Key("f0p1Ms"):_f0p1Ms = (uint32_t)val; return true;
	default:
		break;
	}
	return GadfAlgo_c::Set(key, val);
}

bool GadfPlcAlgo_c::Process(GadfData_c& iData, GadfData_c& oData) {
	int32_t in_used = 0;
	uint8_t* in = (uint8_t*)iData.Data();
	int32_t in_len = iData.Size();
	uint16_t is_lost = 0;
	if (iData.CheckFlag(0x00000001)) {
		is_lost = 0xffff;
	}
	if (!oData.Buf()) {
		oData.Init(_oBuf, _oBufMax);
	}
	oData.Clear();
	uint8_t* out = (uint8_t*)oData.LeftData();
	int32_t out_len = oData.LeftSize();
	bool ret = plc_api_demo_run(_plc_id, in, in_len, &in_used, out, &out_len, is_lost);
	if (!ret) return false;
	iData.Used(in_used);
	oData.Append(out_len);
	return true;
}

bool GadfPlcAlgo_c::DeInit() {
	if (_plc_id) {
		plc_api_demo_deinit(_plc_id);
		_plc_id = 0;
	}
	if (_oBuf) {
		_bp._free(_oBuf);
		_oBuf = 0;
	}
	return true;
}
