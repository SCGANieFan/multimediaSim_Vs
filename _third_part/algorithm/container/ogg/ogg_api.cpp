#include "ogg.h"
#include "ogg_api.h"
#include "OggMuxerApi_c.h"
#include "OggDemuxerApi_c.h"
using namespace ogg_ns;
EXTERNC
{
	OggRet_t ogg_muxer_api_create(OggMuxerApiParam_t * oggMuxerApiParam, void** pHd) {
		return OggMuxerApi_c::Create(oggMuxerApiParam,pHd);
	}
	OggRet_t ogg_muxer_api_receive(void* hd, uint8_t * buf, int32_t len) {
		return OggMuxerApi_c::Receive(hd,buf,len);
	}
	OggRet_t ogg_muxer_api_generate(void* hd, OggPage_t* page) {
		return OggMuxerApi_c::Generate(hd,page);
	}
	OggRet_t ogg_muxer_api_set(void* hd, OggMuxerApiSet_e choose, void* val) {
		return OggMuxerApi_c::Set(hd,choose,val);
	}
	OggRet_t ogg_muxer_api_get(void* hd, OggMuxerApiGet_e choose, void* val) {
		return OggMuxerApi_c::Get(hd,choose,val);
	}
	OggRet_t ogg_muxer_api_destory(void* hd) {
		return OggMuxerApi_c::Destory(hd);
	}

	OggRet_t ogg_demuxer_api_create(OggDeMuxerApiParam_t* oggDeMuxerApiParam, void** pHd) {
		return OggDeMuxerApi_c::Create(oggDeMuxerApiParam, pHd);
	}
	OggRet_t ogg_demuxer_api_receive(void* hd, int32_t len) {
		return OggDeMuxerApi_c::Receive(hd, len);
	}
	OggRet_t ogg_demuxer_api_generate(void* hd, uint8_t* buf, int32_t *len) {
		return OggDeMuxerApi_c::Generate(hd, buf, len);
	}
	OggRet_t ogg_demuxer_api_set(void* hd, OggDeMuxerApiSet_e choose, void* val) {
		return OggDeMuxerApi_c::Set(hd, choose, val);
	}
	OggRet_t ogg_demuxer_api_get(void* hd, OggDeMuxerApiGet_e choose, void* val) {
		return OggDeMuxerApi_c::Get(hd, choose, val);
	}
	OggRet_t ogg_demuxer_api_destory(void* hd) {
		return OggDeMuxerApi_c::Destory(hd);
	}
}