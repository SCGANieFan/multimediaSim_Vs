#include "mp4_api.h"
#include "mp4_demuxer.h"
using namespace GASF_NAME_SPACE;
using namespace mp4_ns;

#if 0
void* mp4_api_muxer_create(Mp4ApiBasePort_t* bp){
	return 0;
}

Mp4ApiRet_t mp4_api_demuxer_open(void* hd) {
	return MP4_API_RET_SUCCESS;
}
Mp4ApiRet_t mp4_api_demuxer_set(void* hd, const char* choose, void* val) {
	return MP4_API_RET_SUCCESS;
}
Mp4ApiRet_t mp4_api_demuxer_get(void* hd, const char* choose, void* val) {
	return MP4_API_RET_SUCCESS;
}
// Mp4ApiRet_t mp4_api_demuxer_receive(void* hd, uint8_t* buf, int32_t* bufByte)
Mp4ApiRet_t mp4_api_demuxer_generate(void* hd, uint8_t* buf, int32_t* bufByte) {
	return MP4_API_RET_SUCCESS;
}
Mp4ApiRet_t mp4_api_demuxer_close(void* hd) {
	return MP4_API_RET_SUCCESS;
}
Mp4ApiRet_t mp4_api_demuxer_destory(void* hd) {
	return MP4_API_RET_SUCCESS;
}

#endif


static Mp4ApiRet_t RetConvert(Mp4Ret_t ret) {
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
	void* mp4_api_demuxer_create(Mp4ApiBasePort_t * bp) {
		return Mp4Demuxer_c::CreateApi<Mp4Demuxer_c>((GasfBasePort_t*)bp);
	}
	Mp4ApiRet_t mp4_api_demuxer_open(void* id) {
		return RetConvert(Mp4Demuxer_c::OpenApi(id));
	}
#if 0
	Mp4ApiRet_t mp4_api_demuxer_receive(void* id, uint8_t * buf, int32_t * len) {
		GasfData_c iData;
		iData.Init(buf, *len, *len);
		//iData.Append(len);
		Mp4Ret_t ret = Mp4Demuxer_c::ReceiveApi(id, iData);
		*len = iData.Used();
		return RetConvert(ret);
	}
#endif
	Mp4ApiRet_t mp4_api_demuxer_generate(void* id, uint8_t * buf, int32_t * len) {
		GasfData_c oData;
		if (len) oData.Init(buf, *len);
		else oData.Init(0, 0);
		//oData.Append(0, oData.LeftSize());
		Mp4Ret_t ret = Mp4Demuxer_c::GenerateApi(id, oData);
		if (len) *len = oData.Size();
		return RetConvert(ret);
	}
	Mp4ApiRet_t mp4_api_demuxer_set(void* id, const char* choose, void* val) {
		return RetConvert(Mp4Demuxer_c::SetApi(id, choose, val));
	}
	Mp4ApiRet_t mp4_api_demuxer_get(void* id, const char* choose, void* val) {
		return RetConvert(Mp4Demuxer_c::GetApi(id, choose, val));
	}
	Mp4ApiRet_t mp4_api_demuxer_close(void* id) {
		return RetConvert(Mp4Demuxer_c::CloseApi(id));
	}
	Mp4ApiRet_t mp4_api_demuxer_destory(void* id) {
		return RetConvert(Mp4Demuxer_c::DestoryeApi(id));
	}
}
