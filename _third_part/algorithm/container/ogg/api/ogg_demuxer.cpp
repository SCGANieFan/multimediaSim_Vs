#include <string.h>
#include "ogg_demuxer.h"
using namespace ogg_api_ns;

OggDeMuxer_c::OggDeMuxer_c() {
}
OggDeMuxer_c::~OggDeMuxer_c() {
}

OggRet_t OggDeMuxer_c::Open(){
	_memory.malloc_cb = _bp.malloc_cb;
	_memory.realloc_cb = _bp.realloc_cb;
	_memory.free_cb = _bp.free_cb;
	ogg_sync_init(&_oggSyncS, &_memory);
	memset(&_oggPacketOld, 0, sizeof(ogg_packet));
	_oggPacketOld.packet = _oBufCache;
	_receiveInfo.bufMax = 2 * 1024;
	return OGG_API_RET_SUCCESS;
}
OggRet_t OggDeMuxer_c::Set(const char* choose, void* val){
	uint64_t key = Str2Key(choose);
	switch (key)
	{
#if 0
	case Str2Key("param"):_param = (OggMuxerApiParam_t*)val; return OGG_API_RET_SUCCESS;
	case Str2Key("gPos"):
		_granulepos = (uint32_t)val;
		return OGG_API_RET_SUCCESS;
	case Str2Key("eos"):
		_isEos = (bool)(uint32_t)val;
		_oggStreamS.e_o_s = _isEos;
		return OGG_API_RET_SUCCESS;
#endif
	default:break;
	}
	return Base_c::Set(choose, val);
}
OggRet_t OggDeMuxer_c::Get(const char* choose, void* val){
	uint64_t key = Str2Key(choose);
	switch (key)
	{
	case Str2Key("recInfo"): 
		{
			_receiveInfo.buf = (uint8_t*)ogg_sync_buffer(&_oggSyncS, _receiveInfo.bufMax);
			*(OggDeMuxerApiReceiveInfo_t*)val = _receiveInfo;
			if (_stage == Stage_e::STAGE_EOS
				|| !_GenerateFinish) {
				((OggDeMuxerApiReceiveInfo_t*)val)->bufMax = 0;
			}
			return OGG_API_RET_SUCCESS;
		}
	case Str2Key("hIdPage"): {
		if (_stage > Stage_e::STAGE_ID_HEAD)
			*(uint32_t*)val = 1;
		else
			*(uint32_t*)val = 0;
		return OGG_API_RET_SUCCESS;
	}
	case Str2Key("idPag"):
		*((OggPage_t**)val) = &_idPage;
		return OGG_API_RET_SUCCESS;
	case Str2Key("hUsrPage"):
		if (_stage > Stage_e::STAGE_USER_COMMENT_HEAD2)
			*(uint32_t*)val = 1;
		else
			*(uint32_t*)val = 0;
		return OGG_API_RET_SUCCESS;
	case Str2Key("usrPage"):
		*((OggPage_t**)val) = &_userPage;
		return OGG_API_RET_SUCCESS;
	default:break;
	}
	return Base_c::Get(choose, val);
}
OggRet_t OggDeMuxer_c::Receive(GaapiData_c& iData){
	int32_t len = iData.Size();
	if (_stage == Stage_e::STAGE_EOS) {
		return OGG_API_RET_SUCCESS;
	}
	if (!_GenerateFinish) {
		return OGG_API_RET_OUT_BUFF_NOT_ENOUGH;
	}
	if (len > _receiveInfo.bufMax
		|| len < 0)
		return OGG_API_RET_INPUT_FAIL;
	int32_t ret = ogg_sync_wrote(&_oggSyncS, len);
	if (ret < 0) {
		LOG_OGG("");
		return OGG_API_RET_FAIL;
	}
	ogg_packet oggPacket;
	while (_stage != Stage_e::STAGE_AUDIO_DATA) {
		switch (_stage)
		{
		case OggDeMuxer_c::Stage_e::STAGE_ID_HEAD:
			ret = ogg_sync_pageout(&_oggSyncS, &_oggPage);
			if (ret != 1) {
				LOG_OGG("");
				return OGG_API_RET_FAIL;
			}
			ret = ogg_stream_init(&_oggStreamS, ogg_page_serialno(&_oggPage), &_memory);
			if (ret) {
				LOG_OGG("");
				return OGG_API_RET_FAIL;
			}
			if (ogg_stream_pagein(&_oggStreamS, &_oggPage) < 0) {
				LOG_OGG("");
				return OGG_API_RET_FAIL;
			}
			if (ogg_stream_packetout(&_oggStreamS, &oggPacket) != 1) {
				LOG_OGG("");
				return OGG_API_RET_FAIL;
			}
			if (sizeof(_idPageCache) < _oggPage.header_len + _oggPage.body_len) {
				return OGG_API_RET_INNER_ERROR;
			}
			memcpy(_idPageCache, _oggPage.header, _oggPage.header_len);
			memcpy(_idPageCache + _oggPage.header_len, _oggPage.body, _oggPage.body_len);
			_idPage.headData = (OggPageHead_t*)_idPageCache;
			_idPage.headLen = _oggPage.header_len;
			_idPage.bodyData = _idPageCache + _oggPage.header_len;
			_idPage.bodyLen = _oggPage.body_len;
			_stage = Stage_e::STAGE_USER_COMMENT_HEAD1;
			break;
		case OggDeMuxer_c::Stage_e::STAGE_USER_COMMENT_HEAD1:
			ret = ogg_sync_pageout(&_oggSyncS, &_oggPage);
			if (ret != 1) {
				LOG_OGG("");
				return OGG_API_RET_SUCCESS;
			}
			if (ogg_stream_pagein(&_oggStreamS, &_oggPage) < 0) {
				LOG_OGG("");
				return OGG_API_RET_FAIL;
			}

			if (sizeof(_userPageCache) < _oggPage.header_len + _oggPage.body_len) {
				return OGG_API_RET_INNER_ERROR;
			}
			memcpy(_userPageCache, _oggPage.header, _oggPage.header_len);
			memcpy(_userPageCache + _oggPage.header_len, _oggPage.body, _oggPage.body_len);
			_userPage.headData = (OggPageHead_t*)_userPageCache;
			_userPage.headLen = _oggPage.header_len;
			_userPage.bodyData = _userPageCache + _oggPage.header_len;
			_userPage.bodyLen = _oggPage.body_len;
			_stage = Stage_e::STAGE_USER_COMMENT_HEAD2;
			break;
		case OggDeMuxer_c::Stage_e::STAGE_USER_COMMENT_HEAD2:
			while (1) {
				ret = ogg_stream_packetout(&_oggStreamS, &oggPacket);
				if (ret == 0) {
					_stage = Stage_e::STAGE_AUDIO_DATA;
					break;
				}
			}
			_stage = Stage_e::STAGE_AUDIO_DATA;
			break;
		case OggDeMuxer_c::Stage_e::STAGE_AUDIO_DATA:

			break;
		default:
			return OGG_API_RET_FAIL;
			break;
		}
	}
	if (_stage == Stage_e::STAGE_AUDIO_DATA) {
		ret = ogg_sync_pageout(&_oggSyncS, &_oggPage);
		if (ret != 1) {
			return OGG_API_RET_MORE_DATA;
		}
		if (ogg_stream_pagein(&_oggStreamS, &_oggPage) < 0) {
			LOG_OGG("");
			return OGG_API_RET_FAIL;
		}

		if (sizeof(_audioDataHeadCache) < _oggPage.header_len)
			return OGG_API_RET_INNER_ERROR;

		memcpy(_audioDataHeadCache, _oggPage.header, _oggPage.header_len);
		_audioDataHead = (OggPageHead_t*)_audioDataHeadCache;
		uint8_t* ptr = &_audioDataHeadCache[27];
		_oPacketNumMax = 0;
		for (int32_t s = 0; s < _audioDataHead->numberPageSegments; s++) {
			if (ptr[s] != 0xff) {
				_oPacketNumMax++;
			}
		}
		_GenerateFinish = false;
		return OGG_API_RET_OUT_BUFF_NOT_ENOUGH;
	}
	return OGG_API_RET_SUCCESS;
}
OggRet_t OggDeMuxer_c::Generate(GaapiData_c& oData){
	if (!oData.LeftSize()) {
		return OGG_API_RET_INPUT_FAIL;
	}
	if (_stage != Stage_e::STAGE_AUDIO_DATA) {
		return OGG_API_RET_MORE_DATA;
	}
	if (_GenerateFinish) {
		return OGG_API_RET_MORE_DATA;
	}
	ogg_packet oggPacket;
	if (_oggPacketOld.bytes) {
		if (_isLastPackNotComplete) {
			if (!(_audioDataHead->headerTypeFlag & 0x01)) {
				return OGG_API_RET_INNER_ERROR;
			}
			int32_t ret = ogg_stream_packetout(&_oggStreamS, &oggPacket);
			if (ret < 0) {
				LOG_OGG("");
				return OGG_API_RET_FAIL;
			}
			_isLastPackNotComplete = false;
			memcpy(_oggPacketOld.packet + _oggPacketOld.bytes, oggPacket.packet, oggPacket.bytes);
			_oggPacketOld.bytes += oggPacket.bytes;

		}
		uint32_t ret = oData.Append(_oggPacketOld.packet, _oggPacketOld.bytes);
		if (ret != _oggPacketOld.bytes) {
			return OGG_API_RET_FAIL;
		}
		_oggPacketOld.bytes = 0;
		return OGG_API_RET_SUCCESS;
	}

	int32_t ret = ogg_stream_packetout(&_oggStreamS, &oggPacket);
	if (ret == 0) {
		if (ogg_page_eos(&_oggPage)) {
			ogg_stream_clear(&_oggStreamS);
			_stage = Stage_e::STAGE_EOS;
		}
		_GenerateFinish = true;
		GaapiData_c idata;
		Receive(idata);
		if (!_GenerateFinish) {
			OggApiRet_t ret = Generate(oData);
			return ret;
		}
		return OGG_API_RET_SUCCESS;
	}
	if (ret < 0) {
		LOG_OGG("");
		return OGG_API_RET_FAIL;
	}
	_oPacketNum = (_oPacketNum + 1) % _oPacketNumMax;

	//LOG_OGG(_printf_cb ,"%d", _oPacketNum);

	if (_oPacketNum == 0) {
		if (_audioDataHeadCache[26 + _audioDataHeadCache[26]] == 0xff
			|| _audioDataHead->headerTypeFlag & 0x01) {
			_isLastPackNotComplete = true;
			memcpy(_oggPacketOld.packet, oggPacket.packet, oggPacket.bytes);
			_oggPacketOld.bytes = oggPacket.bytes;
			return OGG_API_RET_MORE_DATA;
		}
	}
	
	if (oData.LeftSize() < oggPacket.bytes) {
#if 0
		_oggPacketOld.b_o_s = oggPacket.b_o_s;
		_oggPacketOld.e_o_s = oggPacket.e_o_s;
		_oggPacketOld.granulepos = oggPacket.granulepos;
		_oggPacketOld.packetno = oggPacket.packetno;
#endif
		_oggPacketOld.bytes = oggPacket.bytes;
		memcpy(_oggPacketOld.packet, oggPacket.packet, oggPacket.bytes);
		return OGG_API_RET_OUT_BUFF_NOT_ENOUGH;
	}
	oData.Append(oggPacket.packet, oggPacket.bytes);
	return OGG_API_RET_SUCCESS;
}

OggRet_t OggDeMuxer_c::Close() {
	ogg_sync_clear(&_oggSyncS);
	ogg_stream_clear(&_oggStreamS);
	return OGG_API_RET_SUCCESS;
}


EXTERNC void ogg_api_register_ogg_demuxer() {
	gaapi_gaf_register<OggDeMuxer_c>("oggDemux");
}
