#include <string.h>
#include "ogg_api.h"
#include "ogg_api_private.h"
#include "ogg_muxer.h"

using namespace ogg_api_ns;

OggMuxerApi_c::OggMuxerApi_c() {
}
OggMuxerApi_c::~OggMuxerApi_c() {

}

OggRet_t OggMuxerApi_c::Open(){
	LOG_OGG("v%s,%s,(%d,%d,%d,%d,%d),%d",
		OGG_VERSION, _param.mode,
		_param.idParam.version, _param.idParam.channel, _param.idParam.preSkip,
		_param.idParam.sampleRate, _param.idParam.outPutGain,
		_param.page_byte_round);
	if (_param.userComment.vendorStringLen)
		LOG_OGG("vendor:%s", _param.userComment.vendorString);
	if (_param.userComment.userCommentString)
		LOG_OGG("usr comment:%s", _param.userComment.userCommentString);
	if (!_bp.realloc_cb) { return OGG_API_RET_FAIL; }

	_oggMuxer = gaapi_gaf_create("oggMuxer", &_bp);
	if (!_oggMuxer) { 
		LOG_OGG("create fail", _id);
		return OGG_API_RET_NOT_SUPPORT; 
	}
	OggApiRet_t ret = 0;
	ret |= _oggMuxer->Set(Str2Key("param"), (void*)(uint32_t)&_param);
	if (ret != OGG_API_RET_SUCCESS) { 
		LOG_OGG("set fail", _id);
		Close();
		return OGG_API_RET_FAIL; 
	}
	ret  = _oggMuxer->Open();
	if (ret != OGG_API_RET_SUCCESS) {
		LOG_OGG("open fail", _id);
		Close();
		return ret;
	}
	return OGG_API_RET_SUCCESS;
}
OggRet_t OggMuxerApi_c::Set(const char* choose, void* val){
	uint64_t key = Str2Key(choose);
	switch (key)
	{
	case Str2Key("mode"): _param.mode = Str2Key((const char*)val); return OGG_API_RET_SUCCESS;
	case Str2Key("version"): _param.idParam.version = (uint8_t)(uint32_t)val; return OGG_API_RET_SUCCESS;
	case Str2Key("ch"): _param.idParam.channel = (uint8_t)(uint32_t)val; return OGG_API_RET_SUCCESS;
	case Str2Key("preSkip"): _param.idParam.preSkip = (uint16_t)(uint32_t)val; return OGG_API_RET_SUCCESS;
	case Str2Key("fs"): _param.idParam.sampleRate = (uint32_t)val; return OGG_API_RET_SUCCESS;
	case Str2Key("oGain"): _param.idParam.outPutGain = (uint16_t)(uint32_t)val; return OGG_API_RET_SUCCESS;
	case Str2Key("vendor"): 
		_param.userComment.vendorString = (const char*)(uint32_t)val;
		_param.userComment.vendorStringLen = strlen(_param.userComment.vendorString);
		return OGG_API_RET_SUCCESS;
	case Str2Key("comment"):
		_param.userComment.userCommentString = (const char*)(uint32_t)val;
		_param.userComment.userCommentStringLen = strlen(_param.userComment.userCommentString);
		return OGG_API_RET_SUCCESS;
	case Str2Key("pageByte"):_param.page_byte_round = (uint32_t)val; return OGG_API_RET_SUCCESS;
	case Str2Key("gPos"):
		if (_oggMuxer) {
			_oggMuxer->Set(Str2Key("gPos"), val);
			return OGG_API_RET_SUCCESS;
		}
		return OGG_API_RET_FAIL;
	case Str2Key("eos"):
		if (_oggMuxer) {
			_oggMuxer->Set(Str2Key("eos"), val);
			return OGG_API_RET_SUCCESS;
		}
		return OGG_API_RET_FAIL;
	default:
		break;
	}
	return Base_c::Set(choose, val);
}
OggRet_t OggMuxerApi_c::Get(const char* choose, void* val){
	uint64_t key = Str2Key(choose);
	switch (key)
	{
	//case Str2Key("test"):return OGG_API_RET_SUCCESS;
	default:
		break;
	}
	return Base_c::Get(choose, val);
}
#if 0
OggRet_t OggMuxerApi_c::Run(GaapiData_c& iData, GaapiData_c& oData){
	return OGG_API_RET_FAIL;
}
#endif
OggRet_t OggMuxerApi_c::Receive(GaapiData_c& iData){
	OggRet_t ret = _oggMuxer->Receive(iData);
	return ret;
}
OggRet_t OggMuxerApi_c::Generate(GaapiData_c& oData){
	OggRet_t ret = _oggMuxer->Generate(oData);
	return ret;
}
OggRet_t OggMuxerApi_c::Close(){
	_oggMuxer->Close();
	GaapiGaf_c::Destory(_oggMuxer);
	_oggMuxer = 0;
	return OGG_API_RET_SUCCESS;
}


EXTERNC
{
	uint32_t ogg_api_muxer_create(OggApiBasePort_t* bp){
		return OggMuxerApi_c::CreateApi<OggMuxerApi_c>((GaapiBasePort_t*)bp);
	}
	OggApiRet_t ogg_api_muxer_open(uint32_t id) {
		return OggMuxerApi_c::OpenApi(id);
	}
	OggApiRet_t ogg_api_muxer_set(uint32_t id, const char* choose, void* val) {
		return OggMuxerApi_c::SetApi(id, choose, val);
	}
	OggApiRet_t ogg_api_muxer_get(uint32_t id, const char* choose, void* val) {
		return OggMuxerApi_c::GetApi(id, choose, val);
	}
	OggApiRet_t ogg_api_muxer_receive(uint32_t id, uint8_t* buf, int32_t* bufByte) {
		if (!bufByte
			|| !buf) {
			return OGG_API_RET_FAIL;
		}
		GaapiData_c iData;
		uint32_t iSzie = *bufByte;
		iData.Init(buf, iSzie, iSzie);
		OggRet_t ret = OggMuxerApi_c::ReceiveApi(id, iData);
		*bufByte = iData.Used();
		return ret;
	}
	OggApiRet_t ogg_api_muxer_generate(uint32_t id, uint8_t* buf, int32_t* bufByte) {
		if (!buf
			|| !bufByte) {
			return OGG_API_RET_FAIL;
		}
		GaapiData_c oData;
		oData.Init(buf, *bufByte);
		OggRet_t ret = OggMuxerApi_c::GenerateApi(id, oData);
		*bufByte = oData.Size();
		return ret;
	}
	OggApiRet_t ogg_api_muxer_close(uint32_t id) {
		return OggMuxerApi_c::CloseApi(id);
	}
	OggApiRet_t ogg_api_muxer_destory(uint32_t id) {
		return OggMuxerApi_c::DestoryeApi(id);
	}
}


