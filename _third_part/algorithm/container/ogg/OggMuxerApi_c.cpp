#include "OggMuxerApi_c.h"
using namespace ogg_ns;
OggRet_t OggMuxerApi_c::Create(OggMuxerApiParam_t* oggMuxerApiParam, void** pHd) {
	//check
	if (!oggMuxerApiParam
		|| !oggMuxerApiParam->malloc_cb
		|| !oggMuxerApiParam->realloc_cb
		|| !oggMuxerApiParam->free_cb
		) {
		return OGG_RET_INPUT_FAIL;
	}
	
	LOG_OGG(oggMuxerApiParam->printf_cb,"v%s,%d,(%d,%d,%d,%d,%d),%d",
		OGG_VERSION, oggMuxerApiParam->mode,
		oggMuxerApiParam->idParam.version, oggMuxerApiParam->idParam.channel, oggMuxerApiParam->idParam.preSkip,
		oggMuxerApiParam->idParam.sampleRate, oggMuxerApiParam->idParam.outPutGain,
		oggMuxerApiParam->page_byte_round);
	if (oggMuxerApiParam->userComment.vendorStringLen)
		LOG_OGG(oggMuxerApiParam->printf_cb,"%s", oggMuxerApiParam->userComment.vendorString);
	if (oggMuxerApiParam->userComment.userCommentString)
		LOG_OGG(oggMuxerApiParam->printf_cb, "%s", oggMuxerApiParam->userComment.userCommentString);
		
	*pHd = 0;
	int32_t size = sizeof(OggMuxerApi_c);
	OggMuxerApi_c* hd = (OggMuxerApi_c*)oggMuxerApiParam->malloc_cb(size);
	if (!hd) {return OGG_RET_FAIL;}
	new(hd) OggMuxerApi_c();
	hd->_basePorting.malloc_cb = oggMuxerApiParam->malloc_cb;
	hd->_basePorting.realloc_cb = oggMuxerApiParam->realloc_cb;
	hd->_basePorting.free_cb = oggMuxerApiParam->free_cb;
	hd->_basePorting.printf_cb = oggMuxerApiParam->printf_cb;
	//OggBasePortingUpdate(&hd->_basePorting);
	OggRet_t ret = hd->_oggMuxer.Init(oggMuxerApiParam, &hd->_basePorting);
	if (ret != OGG_RET_SUCCESS) {
		Destory(hd);
		return ret;
	}
	*pHd = hd;
	return OGG_RET_SUCCESS;
}
OggRet_t OggMuxerApi_c::Receive(void* hd, uint8_t* buf, int32_t len) {
	OggMuxerApi_c* oggMuxerApi = (OggMuxerApi_c*)hd;
	return oggMuxerApi->_oggMuxer.Receive(buf, len);
}
OggRet_t OggMuxerApi_c::Generate(void* hd, OggPage_t* page) {
	OggMuxerApi_c* oggMuxerApi = (OggMuxerApi_c*)hd;
	if (!page) { 
		return OGG_RET_INPUT_FAIL; 
	}
	return oggMuxerApi->_oggMuxer.Generate(page);
}
OggRet_t OggMuxerApi_c::Set(void* hd, OggMuxerApiSet_e choose, void* val) {
	OggMuxerApi_c* oggMuxerApi = (OggMuxerApi_c*)hd;
	return oggMuxerApi->_oggMuxer.Set(choose, val);
}
OggRet_t OggMuxerApi_c::Get(void* hd, OggMuxerApiGet_e choose, void* val) {
	OggMuxerApi_c* oggMuxerApi = (OggMuxerApi_c*)hd;
	return oggMuxerApi->_oggMuxer.Get(choose, val);
}
OggRet_t OggMuxerApi_c::Destory(void* hd) {
	OggMuxerApi_c* oggMuxerApi = (OggMuxerApi_c*)hd;
	if (oggMuxerApi) {
		oggMuxerApi->_oggMuxer.DeInit();
		oggMuxerApi->~OggMuxerApi_c();
		oggMuxerApi->_basePorting.free_cb(oggMuxerApi);
	}
	return OGG_RET_SUCCESS;
}

