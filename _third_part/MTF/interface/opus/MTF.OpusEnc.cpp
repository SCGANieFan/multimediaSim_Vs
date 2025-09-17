#include "MTF.OpusEnc.h"
#include "MTF.String.h"
#include "MTF.Objects.h"

#include "OpusApi.h"
using namespace OpusApi_ns;

using namespace mtf_ns;
static const char* type_this = "opus_enc";
void mtf_opus_enc_register()
{
	MTF_Objects::Registe<MTF_OpusEnc>(type_this);
	OpusApiMemory_t opusApiMemory;
	opusApiMemory.malloc_cb = Malloc;
	opusApiMemory.realloc_cb = Realloc;
	opusApiMemory.free_cb = Free;
	OpusApi::memory_register(&opusApiMemory);
}
MTF_OpusEnc::MTF_OpusEnc()
{

}

MTF_OpusEnc::~MTF_OpusEnc()
{
	if (_iData.Data())
	{
		_iData.Used(_iData._size);
		MTF_FREE(_iData.Data());
	}
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
	if (_hd)
	{
		if(_hd)
			OpusApi::destory_encoder(_hd);
		_hd = 0;
	}
}

mtf_i32 MTF_OpusEnc::Init()
{	
#if 1
	MTF_PRINT("channels:%d", _ch);
	MTF_PRINT("frameSamples:%d", _frameSamples);
	MTF_PRINT("fsHz:%d", _rate);

	bool haveHead = false;
	OpusApiRet_t ret = OpusApi::create_encoder(&_hd, _rate, _ch, haveHead);
	if (ret != OPUS_API_RET_SUCCESS)
	{
		MTF_PRINT("opus create fail, %d,(%p,%d,%d,%d)", _hd, _rate, _ch, haveHead);
		return -1;
	}
	_frame0p1Ms = _frameSamples * 1000 * 10 / _rate;
	ret = OpusApi::encoder_set(_hd, OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_BIT_RATE, (void*)_bitrate);
	if (ret != OPUS_API_RET_SUCCESS) return -1;
	ret = OpusApi::encoder_set(_hd, OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_FRAME_DURATION_0P1MS, (void*)_frame0p1Ms);
	if (ret != OPUS_API_RET_SUCCESS) return -1;
	ret = OpusApi::encoder_set(_hd, OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_USE_VBR, (void*)false);
	if (ret != OPUS_API_RET_SUCCESS) return -1;
	ret = OpusApi::encoder_set(_hd, OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_COMPLEXITY, (void*)_complexity);
	if (ret != OPUS_API_RET_SUCCESS) return -1;

	MTF_PRINT("create encoder success");
	return 0;
#endif
	//io data
	mtf_i32 size = _frameBytes;
	_iData.Init((mtf_u8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);

	return 0;
}

mtf_i32 MTF_OpusEnc::receive(MTF_Data& iData)
{
	_iData.Append(iData.Data(), iData._size);
	if (iData._flags & MTF_DataFlag_ESO)
		_iData._flags |= MTF_DataFlag_ESO;
	iData.Used(iData._size);
	return 0;
}

mtf_i32 MTF_OpusEnc::generate(MTF_Data*& oData)
{
	if (_iData._flags & MTF_DataFlag_ESO){
		_oData._flags |= MTF_DataFlag_ESO;
		oData = &_oData;
		return 0;
	}
#if 1
	if (_iData._size < _frameBytes) {
		_oData._flags |= MTF_DataFlag_ESO;
		return -1;
	}
	mtf_i32 outLen;
	mtf_i16* iBuff = (mtf_i16*)_iData.Data();
	mtf_u8* oBuff = (mtf_u8*)_oData.LeftData();
	mtf_i32 oSize = _oData.LeftSize();
	OpusApiRet_t ret = OpusApi::encoder_run(_hd, iBuff, _frameSamples, oBuff, &oSize);
	if (ret != OPUS_API_RET_SUCCESS) {
		return -1;
	}
	if (oSize <= 0) {
		return -1;
	}
#endif
	_iData.Used(_frameBytes);
	_oData._size += oSize;
	oData = &_oData;
	return 0;
}

mtf_i32 MTF_OpusEnc::Set(const char* key, mtf_void* val)
{
#if 1
	if (MTF_String::StrCompare(key, "bitrate")) {
		_bitrate = (mtf_i32)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "cpmplexity")) {
		_complexity = (mtf_i32)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "vbr")) {
		_vbr = !!(mtf_i32)val; return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_i32 MTF_OpusEnc::Get(const char* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
