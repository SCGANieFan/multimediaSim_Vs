#include "ogg_api.h"
#include "ogg_demuxer.h"
#include "ogg_muxer.h"
using namespace GASF_NAME_SPACE;
using namespace ogg_ns;

static OggApiRet_t RetConvert(OggRet_t ret) {
#if 0
	switch (ret)
	{
	case OGG_RET_SUCCESS: return OGG_API_RET_SUCCESS;
	case OGG_RET_FAIL: return OGG_API_RET_FAIL;
	case OGG_RET_MORE_DATA: return OGG_API_RET_MORE_DATA;
	case OGG_RET_GENERATE_NOT_FINISH: return OGG_API_RET_GENERATE_NOT_FINISH;
	case OGG_RET_OUT_BUFF_NOT_ENOUGH: return OGG_API_RET_OUT_BUFF_NOT_ENOUGH;
	case OGG_RET_INPUT_FAIL: return OGG_API_RET_INPUT_FAIL;
	case OGG_RET_INNER_ERROR: return OGG_API_RET_INNER_ERROR;
	case OGG_RET_FINISH: return OGG_API_RET_FINISH;
	case OGG_RET_NOT_SUPPORT: return OGG_API_RET_NOT_SUPPORT;
	case OGG_RET_INCOMPLETE: return OGG_API_RET_INCOMPLETE;
	default:return OGG_API_RET_FAIL;
	}
#else
	return ret;
#endif
}

EXTERNC
{
	//demuxer
	void* ogg_api_demuxer_create(OggApiBasePort_t * bp) {
		return OggDemuxer_c::CreateApi<OggDemuxer_c>((GasfBasePort_t*)bp);
	}
	OggApiRet_t ogg_api_demuxer_open(void* id) {
		return RetConvert(OggDemuxer_c::OpenApi(id));
	}
	OggApiRet_t ogg_api_demuxer_receive(void* id, uint8_t * buf, int32_t * len) {
		GasfData_c iData;
		iData.Init(buf, *len, *len);
		//iData.Append(len);
		OggRet_t ret = OggDemuxer_c::ReceiveApi(id, iData);
		*len = iData.Used();
		return RetConvert(ret);
	}
	OggApiRet_t ogg_api_demuxer_generate(void* id, uint8_t * buf, int32_t * len) {
		GasfData_c oData;
		if (len) oData.Init(buf, *len);
		else oData.Init(0, 0);
		//oData.Append(0, oData.LeftSize());
		OggRet_t ret = OggDemuxer_c::GenerateApi(id, oData);
		if (len) *len = oData.Size();
		return RetConvert(ret);
	}
	OggApiRet_t ogg_api_demuxer_set(void* id, const char* choose, void* val) {
		return RetConvert(OggDemuxer_c::SetApi(id, choose, val));
	}
	OggApiRet_t ogg_api_demuxer_get(void* id, const char* choose, void* val) {
		return RetConvert(OggDemuxer_c::GetApi(id, choose, val));
	}
	OggApiRet_t ogg_api_demuxer_close(void* id) {
		return RetConvert(OggDemuxer_c::CloseApi(id));
	}
	OggApiRet_t ogg_api_demuxer_destory(void* id) {
		return RetConvert(OggDemuxer_c::DestoryeApi(id));
	}

	//muxer
	void* ogg_api_muxer_create(OggApiBasePort_t * bp) {
		return OggMuxer_c::CreateApi<OggMuxer_c>((GasfBasePort_t*)bp);
	}
	OggApiRet_t ogg_api_muxer_open(void* id) {
		return RetConvert(OggMuxer_c::OpenApi(id));
	}
	OggApiRet_t ogg_api_muxer_set(void* id, const char* choose, void* val) {
		return RetConvert(OggMuxer_c::SetApi(id, choose, val));
	}
	OggApiRet_t ogg_api_muxer_get(void* id, const char* choose, void* val) {
		return RetConvert(OggMuxer_c::GetApi(id, choose, val));
	}
	OggApiRet_t ogg_api_muxer_receive(void* id, uint8_t * buf, int32_t * bufByte) {
		if (!id
			|| !bufByte
			|| !buf) {
			return OGG_API_RET_FAIL;
		}
		GasfData_c iData;
		uint32_t iSzie = *bufByte;
		iData.Init(buf, iSzie, iSzie);
		OggRet_t ret = OggMuxer_c::ReceiveApi(id, iData);
		*bufByte = iData.Used();
		return RetConvert(ret);
	}
#if 0
	OggApiRet_t ogg_api_muxer_generate(void* id, uint8_t *buf, int32_t *bufByte) {
		if (!buf
			|| !bufByte) {
			return OGG_API_RET_FAIL;
		}
		GasfData_c oData;
		oData.Init(buf, *bufByte);
		OggRet_t ret = OggMuxer_c::GenerateApi(id, oData);
		*bufByte = oData.Size();
		return RetConvert(ret);
	}
#endif
	OggApiRet_t ogg_api_muxer_generate(void* id, OggApiPage_t* page) {
		if (!id
			|| !page) {
			return OGG_API_RET_FAIL;
		}
		OggPage_t page_m;
		GasfData_c oData;
		oData.Init(&page_m, sizeof(OggApiPage_t));
		oData.SetFlag((uint32_t)OggMuxer_c::DataFlag_e::DATA_FLAG_GET_DATA_PAGE);
		OggRet_t ret = OggMuxer_c::GenerateApi(id, oData);
		page->headData = (uint8_t*)page_m.headData;
		page->headByte= page_m.headLen;
		page->bodyData= page_m.bodyData;
		page->bodyByte = page_m.bodyLen;
		return RetConvert(ret);
	}
	OggApiRet_t ogg_api_muxer_close(void* id) {
		return RetConvert(OggMuxer_c::CloseApi(id));
	}
	OggApiRet_t ogg_api_muxer_destory(void* id) {
		return RetConvert(OggMuxer_c::DestoryeApi(id));
	}
}



