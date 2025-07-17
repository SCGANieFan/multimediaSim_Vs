#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <new.h>
#include "Ogg.h"
#include "OggApi.h"

#define LOG_OGG(fmt,...)	if(basePorting_g.printf_cb) basePorting_g.printf_cb("<%s>[%s](%d)" fmt "\n", strrchr(__FILE__,'\\') + 1, __func__, __LINE__, ##__VA_ARGS__)


typedef struct {
	void* (*malloc_cb)(int32_t);
	void* (*calloc_cb)(int32_t, int32_t);
	void* (*realloc_cb)(void *, int32_t);
	void (*free_cb)(void*);
	void (*printf_cb)(const char*, ...);
}BasePorting_t;

static BasePorting_t basePorting_g = { 0 };


void* OggContainerMalloc(int32_t size){
	return basePorting_g.malloc_cb(size);
}
void* OggContainerCalloc(int32_t num, int32_t size){
	return basePorting_g.calloc_cb(num, size);
}
void* OggContainerRealloc(void *ptr, int32_t size){
	return basePorting_g.realloc_cb(ptr, size);
}
void  OggContainerFree(void* ptr){
	return basePorting_g.free_cb(ptr);
}

class OggMuxer_c
{
	enum class Stage_e{
		STAGE_NONE=0,
		STAGE_ID_HEAD,
		STAGE_USER_COMMENT_HEAD,
		STAGE_DATA_HEAD,
	};
public:
	OggMuxer_c() {}
	virtual ~OggMuxer_c() {}
public:
	virtual OggRet_t Init(OggMuxerApiParam_t* param) {
		ogg_sync_init(&_oggSyncS);
		ogg_stream_init(&_oggStreamS, 0x34bd01b1);
		uint8_t packBuf[512];
		int32_t packBufByte = 0;
#if 1
		//id head
		uint8_t* obufTmp = packBuf;
		uint32_t obufByteTmp = 0;
		if (param->mode == OggMuxerApiMode_e::OGG_MUXER_API_MODE_OPUS) {
			memcpy(&obufTmp[obufByteTmp], "OpusHead", 8);
		}
		obufByteTmp += 8;
		memcpy(&obufTmp[obufByteTmp], &param->idParam.version, 1);
		obufByteTmp += 1;
		memcpy(&obufTmp[obufByteTmp], &param->idParam.channel, 1);
		obufByteTmp += 1;
		memcpy(&obufTmp[obufByteTmp], &param->idParam.preSkip, 2);
		obufByteTmp += 2;
		memcpy(&obufTmp[obufByteTmp], &param->idParam.sampleRate, 4);
		obufByteTmp += 4;
		memcpy(&obufTmp[obufByteTmp], &param->idParam.outPutGain, 2);
		obufByteTmp += 2;
#if 0
		memcpy(&obufTmp[obufByteTmp], &mapingFamily, 1); 
#else
		obufTmp[obufByteTmp] = 0;
#endif
		obufByteTmp += 1;
		packBufByte = obufByteTmp;
#endif
		ogg_packet op;
		op.packet = packBuf;
		op.bytes = packBufByte;
		op.b_o_s = 1;
		op.e_o_s = 0;
		op.granulepos = 0;
		op.packetno = 0;
		ogg_stream_packetin(&_oggStreamS, &op);

#if 1
		ogg_page oggPage;
		int32_t ret = ogg_stream_pageout(&_oggStreamS, &oggPage);
		if (ret == 0) {
			return OGG_RET_MORE_DATA;
		}
		memcpy(_idPageCache, oggPage.header, oggPage.header_len);
		memcpy(_idPageCache + oggPage.header_len, oggPage.body, oggPage.body_len);
		_idPage.headData = (OggPageHead_t*)_idPageCache;
		_idPage.headLen = oggPage.header_len;
		_idPage.bodyData = _idPageCache + oggPage.header_len;
		_idPage.bodyLen = oggPage.body_len;
#endif

#if 1
		obufTmp = packBuf;
		obufByteTmp = 0;
		if(param->mode== OggMuxerApiMode_e::OGG_MUXER_API_MODE_OPUS)
			memcpy(&obufTmp[obufByteTmp], "OpusTags", 8);
		obufByteTmp += 8; 
		memcpy(&obufTmp[obufByteTmp], &param->userComment.vendorStringLen, 4);
		obufByteTmp += 4;
		memcpy(&obufTmp[obufByteTmp], param->userComment.vendorString, param->userComment.vendorStringLen);
		obufByteTmp += param->userComment.vendorStringLen;

		uint32_t userCommentListLength = 1;
		memcpy(&obufTmp[obufByteTmp], &userCommentListLength, 4);
		obufByteTmp += 4;

		memcpy(&obufTmp[obufByteTmp], &param->userComment.userCommentStringLen, 4);
		obufByteTmp += 4;

		memcpy(&obufTmp[obufByteTmp], param->userComment.userCommentString, param->userComment.userCommentStringLen);
		obufByteTmp += param->userComment.userCommentStringLen;
		packBufByte = obufByteTmp;
#endif
		op.packet = packBuf;
		op.bytes = packBufByte;
		op.b_o_s = 0;
		op.e_o_s = 0;
		op.granulepos = 0;
		op.packetno = 0;
		ogg_stream_packetin(&_oggStreamS, &op);

#if 1
		ret = ogg_stream_flush_fill(&_oggStreamS, &oggPage, 1);
		if (ret == 0) {
			return OGG_RET_MORE_DATA;
		}

		memcpy(_userCommentPageCache, oggPage.header, oggPage.header_len);
		memcpy(_userCommentPageCache + oggPage.header_len, oggPage.body, oggPage.body_len);
		_userCommentPage.headData = (OggPageHead_t*)_userCommentPageCache;
		_userCommentPage.headLen = oggPage.header_len;
		_userCommentPage.bodyData = _userCommentPageCache + oggPage.header_len;
		_userCommentPage.bodyLen = oggPage.body_len;
#endif
		_stage = Stage_e::STAGE_ID_HEAD;
		return OGG_RET_SUCCESS;
	}
	virtual OggRet_t Receive(uint8_t* buf, int32_t len) {
		if(_isReceiveEnd)
			return OGG_RET_FINISH;
		ogg_packet op;
		op.packet = buf;
		op.bytes = len;
		op.b_o_s = 0;
		op.e_o_s = _isEos ? 1 : 0;
		_isReceiveEnd = _isEos ? 1 : 0;
		op.granulepos = 0;
		op.packetno = 0;
		ogg_stream_packetin(&_oggStreamS, &op);
		return OGG_RET_SUCCESS;
	}
	virtual OggRet_t Generate() {
		if (_isGenrateEnd)
			return OGG_RET_FINISH;

		if (_stage == Stage_e::STAGE_ID_HEAD
			|| _stage == Stage_e::STAGE_USER_COMMENT_HEAD) {
			return OGG_RET_SUCCESS;
		}

		_isGenrateEnd = _isReceiveEnd;
		int32_t ret = ogg_stream_pageout(&_oggStreamS, &_oggPage);
		if (ret == 0) {
			return OGG_RET_MORE_DATA;
		}
		return OGG_RET_SUCCESS;
	}
	virtual OggRet_t Set(OggMuxerApiSet_e choose, void* val) {
		switch (choose)
		{
		case OggMuxerApiSet_e::OGG_MUXER_API_SET_IS_EOS:
			_isEos = (bool)val;
			break;
		default:
			break;
		}

		return OGG_RET_SUCCESS;
	}
	virtual OggRet_t Get(OggMuxerApiGet_e choose, void* val) {
		switch (choose)
		{
		case OggMuxerApiGet_e::OGG_MUXER_API_GET_DATA_PAGE:
			if (_stage == Stage_e::STAGE_ID_HEAD) {
				*(OggPage_t*)val = _idPage;
				_stage = Stage_e::STAGE_USER_COMMENT_HEAD;
			}
			else if (_stage == Stage_e::STAGE_USER_COMMENT_HEAD) {
				*(OggPage_t*)val = _userCommentPage;
				_stage = Stage_e::STAGE_DATA_HEAD;
			}
			else if (_stage == Stage_e::STAGE_DATA_HEAD) {
				((OggPage_t*)val)->headData = (OggPageHead_t*)_oggPage.header;
				((OggPage_t*)val)->headLen= _oggPage.header_len;
				((OggPage_t*)val)->bodyData= _oggPage.body;
				((OggPage_t*)val)->bodyLen = _oggPage.body_len;
			}
			else {
				((OggPage_t*)val)->headData = 0;
				((OggPage_t*)val)->headLen = 0;
				((OggPage_t*)val)->bodyData = 0;
				((OggPage_t*)val)->bodyLen = 0;
			}
			break;
		default:
			break;
		}

		return OGG_RET_SUCCESS;
	}
	virtual OggRet_t DeInit() {
		ogg_stream_clear(&_oggStreamS);
		return OGG_RET_SUCCESS;
	}

public:
	bool _isBos = true;
	bool _isEos = false;
	bool _isReceiveEnd = false;
	bool _isGenrateEnd = false;
	ogg_sync_state   _oggSyncS;
	ogg_page         _oggPage;
	ogg_stream_state _oggStreamS;
	ogg_packet       _oggPacket;
	uint8_t _idPageCache[512];
	OggPage_t _idPage;
	uint8_t _userCommentPageCache[512];
	OggPage_t _userCommentPage;
	Stage_e _stage = Stage_e::STAGE_NONE;
};



class OggMuxerApi_c
{
public:
	OggMuxerApi_c() {}
	~OggMuxerApi_c() {}
public:
	static OggRet_t Create(OggMuxerApiParam_t* oggMuxerApiParam, void** pHd) {
		* pHd = 0;
		int32_t size = sizeof(OggMuxerApi_c);
		OggMuxerApi_c* hd = (OggMuxerApi_c*)oggMuxerApiParam->malloc_cb(size);
		if (!hd) {
			return OGG_RET_FAIL;
		}
		new(hd) OggMuxerApi_c();
		if(basePorting_g.malloc_cb!=oggMuxerApiParam->malloc_cb){
			basePorting_g.malloc_cb = *oggMuxerApiParam->malloc_cb;
			basePorting_g.calloc_cb = *oggMuxerApiParam->calloc_cb;
			basePorting_g.realloc_cb = *oggMuxerApiParam->realloc_cb;
			basePorting_g.free_cb = *oggMuxerApiParam->free_cb;
			basePorting_g.printf_cb = *oggMuxerApiParam->printf_cb;
		}
		hd->_basePorting.malloc_cb = oggMuxerApiParam->malloc_cb;
		hd->_basePorting.calloc_cb = oggMuxerApiParam->calloc_cb;
		hd->_basePorting.realloc_cb = oggMuxerApiParam->realloc_cb;
		hd->_basePorting.free_cb= oggMuxerApiParam->free_cb;
		hd->_basePorting.printf_cb= oggMuxerApiParam->printf_cb;
		OggRet_t ret = hd->_oggMuxer.Init(oggMuxerApiParam);
		if (ret != OGG_RET_SUCCESS) {
			return ret;
		}
		*pHd = hd;
		return OGG_RET_SUCCESS;
	}
	static OggRet_t Receive(void* hd, uint8_t* buf, int32_t len) {
		OggMuxerApi_c* oggMuxerApi = (OggMuxerApi_c*)hd;
		return oggMuxerApi->_oggMuxer.Receive(buf, len);
	}
	static OggRet_t Generate(void* hd) {
		OggMuxerApi_c* oggMuxerApi = (OggMuxerApi_c*)hd;
		return oggMuxerApi->_oggMuxer.Generate();
	}
	static OggRet_t Set(void* hd, OggMuxerApiSet_e choose, void* val) {
		OggMuxerApi_c* oggMuxerApi = (OggMuxerApi_c*)hd;
		return oggMuxerApi->_oggMuxer.Set(choose, val);
	}
	static OggRet_t Get(void* hd, OggMuxerApiGet_e choose, void* val) {
		OggMuxerApi_c* oggMuxerApi = (OggMuxerApi_c*)hd;
		return oggMuxerApi->_oggMuxer.Get(choose, val);
	}
	static OggRet_t Destory(void* hd) {
		OggMuxerApi_c* oggMuxerApi = (OggMuxerApi_c*)hd;
		if (oggMuxerApi) {
			oggMuxerApi->_oggMuxer.DeInit();
			oggMuxerApi->~OggMuxerApi_c();
			oggMuxerApi->_basePorting.free_cb(oggMuxerApi);
		}
		return OGG_RET_SUCCESS;
	}
public:
	BasePorting_t _basePorting;
	OggMuxer_c _oggMuxer;
};


#if 1
class OggDeMuxer_c
{
public:
	enum class Stage_e {
		STAGE_ID_HEAD = 0,
		STAGE_USER_COMMENT_HEAD1,
		STAGE_USER_COMMENT_HEAD2,
		STAGE_AUDIO_DATA,
		STAGE_EOS,
	};
public:
	OggDeMuxer_c() {}
	~OggDeMuxer_c() {}
public:
	virtual OggRet_t Init(BasePorting_t *basePort) {
		ogg_sync_init(&_oggSyncS);
		_basePort = basePort;
		memset(&_oggPacketOld, 0, sizeof(ogg_packet));
		_oggPacketOld.packet = _oBufCache;
		_receiveInfo.bufMax = 2*1024;
		return OGG_RET_SUCCESS;
	}
	virtual OggRet_t Receive(int32_t len) {
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
			LOG_OGG("");
			return OGG_RET_FAIL;
		}
		ogg_packet oggPacket;
		while (_stage != Stage_e::STAGE_AUDIO_DATA) {
			switch (_stage)
			{
			case OggDeMuxer_c::Stage_e::STAGE_ID_HEAD:
				ret = ogg_sync_pageout(&_oggSyncS, &_oggPage);
				if (ret != 1) {
					LOG_OGG("");
					return OGG_RET_FAIL;
				}
				ret = ogg_stream_init(&_oggStreamS, ogg_page_serialno(&_oggPage));
				if (ret) {
					LOG_OGG("");
					return OGG_RET_FAIL;
				}
				if (ogg_stream_pagein(&_oggStreamS, &_oggPage) < 0) {
					LOG_OGG("");
					return OGG_RET_FAIL;
				}
				if (ogg_stream_packetout(&_oggStreamS, &oggPacket) != 1) {
					LOG_OGG("");
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
					LOG_OGG("");
					return OGG_RET_SUCCESS;
				}
				if (ogg_stream_pagein(&_oggStreamS, &_oggPage) < 0) {
					LOG_OGG("");
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
				LOG_OGG("");
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
	virtual OggRet_t Generate(uint8_t* buf, int32_t *len) {
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
					LOG_OGG("");
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
			return OGG_RET_SUCCESS;
		}
		if (ret < 0) {
			LOG_OGG("");
			return OGG_RET_FAIL;
		}
		_oPacketNum =(++_oPacketNum) % _oPacketNumMax;
		//LOG_OGG("%d", _oPacketNum);

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
	virtual OggRet_t Set(OggDeMuxerApiSet_e choose, void* val) {
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
	virtual OggRet_t Get(OggDeMuxerApiGet_e choose, void* val) {
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
	virtual OggRet_t DeInit() {
		ogg_sync_clear(&_oggSyncS);
		ogg_stream_clear(&_oggStreamS);
		return OGG_RET_SUCCESS;
	}
public:
	ogg_sync_state   _oggSyncS;
	ogg_page         _oggPage;
	ogg_stream_state _oggStreamS;
	uint8_t _idMagic[8];
	OggMuxerApiIdParam_t _idParam;
	Stage_e _stage = Stage_e::STAGE_ID_HEAD;
	BasePorting_t *_basePort;
	
	uint8_t _idPageCache[512];
	OggPage_t _idPage;
	uint8_t _userPageCache[512];
	OggPage_t _userPage;
	uint8_t _audioDataHeadCache[512];
	OggPageHead_t *_audioDataHead;
	int32_t _oPacketNum = 0;
	int32_t _oPacketNumMax = 0;
	uint8_t _oBufCache[512];
	ogg_packet _oggPacketOld;
	OggDeMuxerApiReceiveInfo_t _receiveInfo;
	bool _GenerateFinish = true;
	bool _isLastPackNotComplete = false;
};




class OggDeMuxerApi_c
{
public:
	OggDeMuxerApi_c() {}
	~OggDeMuxerApi_c() {}
public:
	static OggRet_t Create(OggDeMuxerApiParam_t* oggDeMuxerApiParam, void** pHd) {
		int32_t size = sizeof(OggDeMuxerApi_c);
		OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)oggDeMuxerApiParam->malloc_cb(size);
		if (!oggDeMuxerApi) {
			return OGG_RET_FAIL;
		}
		if(basePorting_g.malloc_cb!=oggDeMuxerApiParam->malloc_cb){
			basePorting_g.malloc_cb = oggDeMuxerApiParam->malloc_cb;
			basePorting_g.calloc_cb = oggDeMuxerApiParam->calloc_cb;
			basePorting_g.realloc_cb = oggDeMuxerApiParam->realloc_cb;
			basePorting_g.free_cb = oggDeMuxerApiParam->free_cb;
			basePorting_g.printf_cb = oggDeMuxerApiParam->printf_cb;
		}
		new(oggDeMuxerApi) OggDeMuxerApi_c();
		oggDeMuxerApi->_basePorting.malloc_cb = oggDeMuxerApiParam->malloc_cb;
		oggDeMuxerApi->_basePorting.calloc_cb = oggDeMuxerApiParam->calloc_cb;
		oggDeMuxerApi->_basePorting.realloc_cb = oggDeMuxerApiParam->realloc_cb;
		oggDeMuxerApi->_basePorting.free_cb = oggDeMuxerApiParam->free_cb;
		oggDeMuxerApi->_basePorting.printf_cb = oggDeMuxerApiParam->printf_cb;

		size = sizeof(OggDeMuxer_c);
		oggDeMuxerApi->_oggDeMuxer = (OggDeMuxer_c*)oggDeMuxerApi->_basePorting.malloc_cb(size);
		if (!oggDeMuxerApi->_oggDeMuxer) {
			return OGG_RET_FAIL;
		}
		new(oggDeMuxerApi->_oggDeMuxer) OggDeMuxer_c();
		oggDeMuxerApi->_oggDeMuxer->Init(&oggDeMuxerApi->_basePorting);
		*pHd = oggDeMuxerApi;
		return OGG_RET_SUCCESS;
	}
	static OggRet_t Receive(void* hd, int32_t len) {
		OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)hd;
		return oggDeMuxerApi->_oggDeMuxer->Receive(len);
	}
	static OggRet_t Generate(void* hd, uint8_t* buf, int32_t *len) {
		OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)hd;
		return oggDeMuxerApi->_oggDeMuxer->Generate(buf, len);
	}
	static OggRet_t Set(void* hd, OggDeMuxerApiSet_e choose, void* val) {
		OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)hd;
		return oggDeMuxerApi->_oggDeMuxer->Set(choose, val);
	}
	static OggRet_t Get(void* hd, OggDeMuxerApiGet_e choose, void* val) {
		OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)hd;
		return oggDeMuxerApi->_oggDeMuxer->Get(choose, val);
	}
	static OggRet_t Destory(void* hd) {
		OggDeMuxerApi_c* oggDeMuxerApi = (OggDeMuxerApi_c*)hd;
		if (oggDeMuxerApi) {
			if (oggDeMuxerApi->_oggDeMuxer) {
				OggRet_t ret = oggDeMuxerApi->_oggDeMuxer->DeInit();
				if (ret != OGG_RET_SUCCESS) {
					return ret;
				}
				oggDeMuxerApi->_oggDeMuxer->~OggDeMuxer_c();
				oggDeMuxerApi->_basePorting.free_cb(oggDeMuxerApi->_oggDeMuxer);
			}
			oggDeMuxerApi->~OggDeMuxerApi_c();
			oggDeMuxerApi->_basePorting.free_cb(oggDeMuxerApi);
		}
		return OGG_RET_SUCCESS;
	}
public:
	BasePorting_t _basePorting;
	OggDeMuxer_c* _oggDeMuxer = 0;
};

#endif

EXTERNC
{
	OggRet_t OggMuxerApiCreate(OggMuxerApiParam_t * oggMuxerApiParam, void** pHd) {
		return OggMuxerApi_c::Create(oggMuxerApiParam,pHd);
	}
	OggRet_t OggMuxerApiReceive(void* hd, uint8_t * buf, int32_t len) {
		return OggMuxerApi_c::Receive(hd,buf,len);
	}
	OggRet_t OggMuxerApiGenerate(void* hd) {
		return OggMuxerApi_c::Generate(hd);
	}
	OggRet_t OggMuxerApiSet(void* hd, OggMuxerApiSet_e choose, void* val) {
		return OggMuxerApi_c::Set(hd,choose,val);
	}
	OggRet_t OggMuxerApiGet(void* hd, OggMuxerApiGet_e choose, void* val) {
		return OggMuxerApi_c::Get(hd,choose,val);
	}
	OggRet_t OggMuxerApiDestory(void* hd) {
		return OggMuxerApi_c::Destory(hd);
	}

	OggRet_t OggDeMuxerApiCreate(OggDeMuxerApiParam_t* oggDeMuxerApiParam, void** pHd) {
		return OggDeMuxerApi_c::Create(oggDeMuxerApiParam, pHd);
	}
	OggRet_t OggDeMuxerApiReceive(void* hd, int32_t len) {
		return OggDeMuxerApi_c::Receive(hd, len);
	}
	OggRet_t OggDeMuxerApiGenerate(void* hd, uint8_t* buf, int32_t *len) {
		return OggDeMuxerApi_c::Generate(hd, buf, len);
	}
	OggRet_t OggDeMuxerApiSet(void* hd, OggDeMuxerApiSet_e choose, void* val) {
		return OggDeMuxerApi_c::Set(hd, choose, val);
	}
	OggRet_t OggDeMuxerApiGet(void* hd, OggDeMuxerApiGet_e choose, void* val) {
		return OggDeMuxerApi_c::Get(hd, choose, val);
	}
	OggRet_t OggDeMuxerApiDestory(void* hd) {
		return OggDeMuxerApi_c::Destory(hd);
	}
}