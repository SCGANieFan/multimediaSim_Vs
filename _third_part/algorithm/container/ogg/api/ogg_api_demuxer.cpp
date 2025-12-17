#include "ogg_api_private.h"
using namespace ogg_api_ns;
OggDeMuxerApi_c::OggDeMuxerApi_c() {
}
OggDeMuxerApi_c::~OggDeMuxerApi_c() {
}
OggRet_t OggDeMuxerApi_c::Open(){
	if (!_bp.realloc_cb) { return OGG_API_RET_FAIL; }
	LOG_OGG("v%s", OGG_VERSION);
	if (_oggDeMuxer) {
		LOG_OGG("do clsoe befor open,%u,%p,%p,", _id, this, _oggDeMuxer);
		return OGG_API_RET_FAIL;
	}
	_oggDeMuxer = gaapi_gaf_create("oggDemux", &_bp);
	if (!_oggDeMuxer) {
		LOG_OGG("create fail, %u,%p", _id, _bp.malloc_cb);
		return OGG_API_RET_NOT_SUPPORT;
	}
	OggRet_t ret;
	ret = _oggDeMuxer->Open();
	if (ret != OGG_API_RET_SUCCESS) {
		LOG_OGG("open fail, %u", _id);
		Close();
		return ret;
	}
	return OGG_API_RET_SUCCESS;
}
OggRet_t OggDeMuxerApi_c::Set(const char* choose, void* val){
	uint32_t key = Str2Key(choose);
#if 0
	switch (key)
	{
		//case Str2Key("test"):return OGG_API_RET_SUCCESS;
	default:
		break;
	}
#endif
	if (_oggDeMuxer) {
		if(_oggDeMuxer->Set(key, val) == OGG_API_RET_SUCCESS) return OGG_API_RET_SUCCESS;
	}
	if(Base_c::Set(choose, val) == GAAPI_RET_SUCCESS) return OGG_API_RET_SUCCESS;
	return OGG_API_RET_FAIL;
}
OggRet_t OggDeMuxerApi_c::Get(const char* choose, void* val){
	uint32_t key = Str2Key(choose);
#if 0
	switch (key)
	{
		//case Str2Key("test"):return OGG_API_RET_SUCCESS;
	default:
		break;
	}
#endif
	if (_oggDeMuxer) {
		if (_oggDeMuxer->Get(key, val) == OGG_API_RET_SUCCESS) return OGG_API_RET_SUCCESS;
	} 
	if(Base_c::Get(choose, val) == GAAPI_RET_SUCCESS) return OGG_API_RET_SUCCESS;
	return OGG_API_RET_FAIL;
}
OggRet_t OggDeMuxerApi_c::Receive(GaapiData_c& iData){
	return _oggDeMuxer->Receive(iData);
}
OggRet_t OggDeMuxerApi_c::Generate(GaapiData_c& oData){
	return _oggDeMuxer->Generate(oData);
}
OggRet_t OggDeMuxerApi_c::Close(){
	_oggDeMuxer->Close();
	GaapiGaf_c::Destory(_oggDeMuxer);
	_oggDeMuxer = 0;
	return OGG_API_RET_SUCCESS;
}

EXTERNC
{
	uint32_t ogg_api_demuxer_create(OggApiBasePort_t* bp) {
		return OggDeMuxerApi_c::CreateApi<OggDeMuxerApi_c>((GaapiBasePort_t*)bp);
	}
	OggApiRet_t ogg_api_demuxer_open(uint32_t id) {
		return OggDeMuxerApi_c::OpenApi(id);
	}
	OggApiRet_t ogg_api_demuxer_receive(uint32_t id, int32_t len) {
		GaapiData_c iData;
		iData.Init(0, len);
		iData.Append(len);
		return OggDeMuxerApi_c::ReceiveApi(id, iData);
	}
	OggApiRet_t ogg_api_demuxer_generate(uint32_t id, uint8_t* buf, int32_t *len) {
		GaapiData_c oData;
		oData.Init(0, *len);
		oData.Append(0, oData.LeftSize());
		OggApiRet_t ret = OggDeMuxerApi_c::GenerateApi(id, oData);
		*len = oData.Size();
		return ret;
	}
	OggApiRet_t ogg_api_demuxer_set(uint32_t id, const char* choose, void* val) {
		return OggDeMuxerApi_c::SetApi(id, choose, val);
	}
	OggApiRet_t ogg_api_demuxer_get(uint32_t id, const char* choose, void* val) {
		return OggDeMuxerApi_c::GetApi(id, choose, val);
	}
	OggApiRet_t ogg_api_demuxer_close(uint32_t id) {
		return OggDeMuxerApi_c::CloseApi(id);
	}
	OggApiRet_t ogg_api_demuxer_destory(uint32_t id) {
		return OggDeMuxerApi_c::DestoryeApi(id);
	}
}