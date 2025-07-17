#include "OggDemuxerApi_c.h"
using namespace ogg_ns;
OggRet_t OggDeMuxerApi_c::Create(OggDeMuxerApiParam_t* oggDeMuxerApiParam, void** pHd) {
	int32_t size = sizeof(OggDeMuxerApi_c);
	OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)oggDeMuxerApiParam->malloc_cb(size);
	if (!oggDeMuxerApi) {
		return OGG_RET_FAIL;
	}

	new(oggDeMuxerApi) OggDeMuxerApi_c();
	oggDeMuxerApi->_basePorting.malloc_cb = oggDeMuxerApiParam->malloc_cb;
	oggDeMuxerApi->_basePorting.realloc_cb = oggDeMuxerApiParam->realloc_cb;
	oggDeMuxerApi->_basePorting.free_cb = oggDeMuxerApiParam->free_cb;
	oggDeMuxerApi->_basePorting.printf_cb = oggDeMuxerApiParam->printf_cb;

	size = sizeof(OggDeMuxer_c);
	oggDeMuxerApi->_oggDeMuxer.Init(&oggDeMuxerApi->_basePorting);
	*pHd = oggDeMuxerApi;
	return OGG_RET_SUCCESS;
}
OggRet_t OggDeMuxerApi_c::Receive(void* hd, int32_t len) {
	OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)hd;
	return oggDeMuxerApi->_oggDeMuxer.Receive(len);
}
OggRet_t OggDeMuxerApi_c::Generate(void* hd, uint8_t* buf, int32_t *len) {
	OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)hd;
	return oggDeMuxerApi->_oggDeMuxer.Generate(buf, len);
}
OggRet_t OggDeMuxerApi_c::Set(void* hd, OggDeMuxerApiSet_e choose, void* val) {
	OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)hd;
	return oggDeMuxerApi->_oggDeMuxer.Set(choose, val);
}
OggRet_t OggDeMuxerApi_c::Get(void* hd, OggDeMuxerApiGet_e choose, void* val) {
	OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)hd;
	return oggDeMuxerApi->_oggDeMuxer.Get(choose, val);
}
OggRet_t OggDeMuxerApi_c::Destory(void* hd) {
	OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)hd;
	if (oggDeMuxerApi) {
		oggDeMuxerApi->_oggDeMuxer.DeInit();
		oggDeMuxerApi->~OggDeMuxerApi_c();
		oggDeMuxerApi->_basePorting.free_cb(oggDeMuxerApi);
	}
	return OGG_RET_SUCCESS;
}
