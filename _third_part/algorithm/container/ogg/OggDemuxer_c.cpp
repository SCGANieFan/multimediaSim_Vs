#include "OggDemuxer_c.h"
using namespace ogg_ns;
OggRet_t OggDeMuxer_c::Init(OggBasePorting_t *basePort) {
	_MM.Init((OggBasePorting_t*)basePort);
	_printf_cb = basePort->printf_cb;
	ogg_sync_init(&_oggSyncS, &_MM);
	memset(&_oggPacketOld, 0, sizeof(ogg_packet));
	_oggPacketOld.packet = _oBufCache;
	_receiveInfo.bufMax = 2*1024;
	return OGG_RET_SUCCESS;
}
OggRet_t OggDeMuxer_c::Receive(int32_t len) {
	if (_stage == Stage_e::STAGE_EOS) {
		return OGG_RET_SUCCESS;
	}
	if (!_GenerateFinish) {
		return OGG_RET_GENERATE_NOT_FINISH;
	}
	if (len > _receiveInfo.bufMax
		|| len < 0)
		return OGG_RET_INPUT_FAIL;
	int32_t ret = ogg_sync_wrote(&_oggSyncS, len);
	if (ret < 0) {
		LOG_OGG(_printf_cb ,"");
		return OGG_RET_FAIL;
	}
	ogg_packet oggPacket;
	while (_stage != Stage_e::STAGE_AUDIO_DATA) {
		switch (_stage)
		{
		case OggDeMuxer_c::Stage_e::STAGE_ID_HEAD:
			ret = ogg_sync_pageout(&_oggSyncS, &_oggPage);
			if (ret != 1) {
				LOG_OGG(_printf_cb ,"");
				return OGG_RET_FAIL;
			}
			ret = ogg_stream_init(&_oggStreamS, ogg_page_serialno(&_oggPage),&_MM);
			if (ret) {
				LOG_OGG(_printf_cb ,"");
				return OGG_RET_FAIL;
			}
			if (ogg_stream_pagein(&_oggStreamS, &_oggPage) < 0) {
				LOG_OGG(_printf_cb ,"");
				return OGG_RET_FAIL;
			}
			if (ogg_stream_packetout(&_oggStreamS, &oggPacket) != 1) {
				LOG_OGG(_printf_cb ,"");
				return OGG_RET_FAIL;
			}
			if (sizeof(_idPageCache) < _oggPage.header_len + _oggPage.body_len) {
				return OGG_RET_INNER_ERROR;
			}
			memcpy(_idPageCache, _oggPage.header, _oggPage.header_len);
			memcpy(_idPageCache + _oggPage.header_len, _oggPage.body, _oggPage.body_len);
			_idPage.headData = (OggPageHead_t*)_idPageCache;
			_idPage.headLen= _oggPage.header_len;
			_idPage.bodyData = _idPageCache + _oggPage.header_len;
			_idPage.bodyLen = _oggPage.body_len;
			_stage = Stage_e::STAGE_USER_COMMENT_HEAD1;
			break;
		case OggDeMuxer_c::Stage_e::STAGE_USER_COMMENT_HEAD1:
			ret = ogg_sync_pageout(&_oggSyncS, &_oggPage);
			if (ret != 1) {
				LOG_OGG(_printf_cb ,"");
				return OGG_RET_SUCCESS;
			}
			if (ogg_stream_pagein(&_oggStreamS, &_oggPage) < 0) {
				LOG_OGG(_printf_cb ,"");
				return OGG_RET_FAIL;
			}

			if (sizeof(_userPageCache) < _oggPage.header_len + _oggPage.body_len) {
				return OGG_RET_INNER_ERROR;
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
			return OGG_RET_FAIL;
			break;
		}
	}
	if (_stage == Stage_e::STAGE_AUDIO_DATA) {
		ret = ogg_sync_pageout(&_oggSyncS, &_oggPage);
		if (ret != 1) {
			return OGG_RET_MORE_DATA;
		}
		if (ogg_stream_pagein(&_oggStreamS, &_oggPage) < 0) {
			LOG_OGG(_printf_cb ,"");
			return OGG_RET_FAIL;
		}
		
		if (sizeof(_audioDataHeadCache) < _oggPage.header_len)
			return OGG_RET_INNER_ERROR;
		
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
		return OGG_RET_GENERATE_NOT_FINISH;
	}
	return OGG_RET_SUCCESS;
}
OggRet_t OggDeMuxer_c::Generate(uint8_t* buf, int32_t *len) {
	if (!len) {
		return OGG_RET_INPUT_FAIL;
	}
	int32_t oLen = *len;
	*len = 0;
	if (_stage != Stage_e::STAGE_AUDIO_DATA){
		return OGG_RET_MORE_DATA;
	}
	if (_GenerateFinish) {
		return OGG_RET_MORE_DATA;
	}
	ogg_packet oggPacket;
	if (_oggPacketOld.bytes) {
		if (_isLastPackNotComplete) {
			if (!(_audioDataHead->headerTypeFlag & 0x01)) {
				return OGG_RET_INNER_ERROR;
			}
			int32_t ret = ogg_stream_packetout(&_oggStreamS, &oggPacket);
			if (ret < 0) {
				LOG_OGG(_printf_cb ,"");
				return OGG_RET_FAIL;
			}
			_isLastPackNotComplete = false;
			memcpy(_oggPacketOld.packet + _oggPacketOld.bytes, oggPacket.packet, oggPacket.bytes);
			_oggPacketOld.bytes += oggPacket.bytes;

		}
		memcpy(buf, _oggPacketOld.packet, _oggPacketOld.bytes);
		*len = _oggPacketOld.bytes;
		_oggPacketOld.bytes = 0;
		return OGG_RET_SUCCESS;
	}

	int32_t ret = ogg_stream_packetout(&_oggStreamS, &oggPacket);
	if (ret == 0) {
		if (ogg_page_eos(&_oggPage)) {
			ogg_stream_clear(&_oggStreamS);
			_stage = Stage_e::STAGE_EOS;
		}
		_GenerateFinish = true;
		Receive(0);
		if (!_GenerateFinish) {
			OggRet_t ret = Generate(buf, &oLen);
			*len = oLen;
			return ret;
		}
		return OGG_RET_SUCCESS;
	}
	if (ret < 0) {
		LOG_OGG(_printf_cb ,"");
		return OGG_RET_FAIL;
	}
	_oPacketNum =(++_oPacketNum) % _oPacketNumMax;
	//LOG_OGG(_printf_cb ,"%d", _oPacketNum);

	if (_oPacketNum == 0) {
		if (_audioDataHeadCache[26 + _audioDataHeadCache[26]] == 0xff
			|| _audioDataHead->headerTypeFlag & 0x01) {
			_isLastPackNotComplete = true;
			memcpy(_oggPacketOld.packet, oggPacket.packet, oggPacket.bytes);
			_oggPacketOld.bytes = oggPacket.bytes;
			return OGG_RET_MORE_DATA;
		}
	}

	if (oLen<oggPacket.bytes) {
#if 0
		_oggPacketOld.b_o_s = oggPacket.b_o_s;
		_oggPacketOld.e_o_s = oggPacket.e_o_s;
		_oggPacketOld.granulepos = oggPacket.granulepos;
		_oggPacketOld.packetno = oggPacket.packetno;
#endif
		_oggPacketOld.bytes = oggPacket.bytes;
		memcpy(_oggPacketOld.packet, oggPacket.packet, oggPacket.bytes);
		return OGG_RET_OUT_BUFF_NOT_ENOUGH;
	}
	memcpy(buf, oggPacket.packet, oggPacket.bytes);
	*len = oggPacket.bytes;
	return OGG_RET_SUCCESS;
}
OggRet_t OggDeMuxer_c::Set(OggDeMuxerApiSet_e choose, void* val) {
#if 0
	switch (choose)
	{
	case OggDeMuxerApiSet_e::OGG_DEMUXER_API_SET_IS_EOS:
		break;
	default:
		break;
	}
#endif
	return OGG_RET_SUCCESS;
}
OggRet_t OggDeMuxer_c::Get(OggDeMuxerApiGet_e choose, void* val) {
	switch (choose)
	{
	case OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_RECEIVE_INFO:
		_receiveInfo.buf = (uint8_t*)ogg_sync_buffer(&_oggSyncS, _receiveInfo.bufMax);
		*(OggDeMuxerApiReceiveInfo_t*)val = _receiveInfo;
		if (_stage == Stage_e::STAGE_EOS
			|| !_GenerateFinish) {
			((OggDeMuxerApiReceiveInfo_t*)val)->bufMax = 0;
		}
		break;
	case OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_HAS_ID_PAGE:
		if(_stage> Stage_e::STAGE_ID_HEAD)
			*(uint32_t*)val = 1;
		else
			*(uint32_t*)val = 0;
		break;
	case OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_ID_PAGE:
		*((OggPage_t**)val) = &_idPage;
		break;
	case OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_HAS_USER_PAGE:
		if (_stage > Stage_e::STAGE_USER_COMMENT_HEAD2)
			*(uint32_t*)val = 1;
		else
			*(uint32_t*)val = 0;
		break;
	case OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_USER_PAGE:
		*((OggPage_t**)val) = &_userPage;
		break;
	default:
		break;
	}
	return OGG_RET_SUCCESS;
}
OggRet_t OggDeMuxer_c::DeInit() {
	ogg_sync_clear(&_oggSyncS);
	ogg_stream_clear(&_oggStreamS);
	return OGG_RET_SUCCESS;
}



