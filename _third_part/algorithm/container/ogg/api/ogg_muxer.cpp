#include <string.h>
#include "ogg_muxer.h"

using namespace ogg_ns;

OggMuxer_c::OggMuxer_c() {
}
OggMuxer_c::~OggMuxer_c() {

}

OggRet_t OggMuxer_c::Open() {
	LOG_OGG("v%s", OGG_VERSION);
	OggRet_t ret = 0;
	if (!_bp.malloc_cb
		|| !_bp.free_cb
		|| !_bp.realloc_cb
		) {
		return OGG_RET_FAIL;
	}
	LOG_OGG("(%d,%d,%d,%d,%d),%d",
		_param.idParam.version, _param.idParam.channel, _param.idParam.preSkip,
		_param.idParam.sampleRate, _param.idParam.outPutGain,
		_param.page_byte_round);
	if (_param.userComment.vendorStringLen)
		LOG_OGG("vendor:%s", _param.userComment.vendorString);
	if (_param.userComment.userCommentString)
		LOG_OGG("usr comment:%s", _param.userComment.userCommentString);
	ogg_memory_t memory;
	memory.malloc_cb = _bp.malloc_cb;
	memory.realloc_cb = _bp.realloc_cb;
	memory.free_cb = _bp.free_cb;
	ogg_sync_init(&_oggSyncS, &memory);
	ogg_stream_init(&_oggStreamS, 0x34bd01b1, &memory);
	if (_param.page_byte_round) _page_out_fill_byte = _param.page_byte_round;
	uint8_t packBuf[512];
	int32_t packBufByte = 0;

	//id head
	uint8_t* obufTmp = packBuf;
	uint32_t obufTmpByte = 0;
	if (_param.mode == Str2Key("opus")) {
		gasf_memcpy(&obufTmp[obufTmpByte], "OpusHead", 8);
	}
	else {
		gasf_memcpy(&obufTmp[obufTmpByte], "unknowns", 8);
	}
	obufTmpByte += 8;
	gasf_memcpy(&obufTmp[obufTmpByte], &_param.idParam.version, 1);
	obufTmpByte += 1;
	gasf_memcpy(&obufTmp[obufTmpByte], &_param.idParam.channel, 1);
	obufTmpByte += 1;
	gasf_memcpy(&obufTmp[obufTmpByte], &_param.idParam.preSkip, 2);
	obufTmpByte += 2;
	gasf_memcpy(&obufTmp[obufTmpByte], &_param.idParam.sampleRate, 4);
	obufTmpByte += 4;
	gasf_memcpy(&obufTmp[obufTmpByte], &_param.idParam.outPutGain, 2);
	obufTmpByte += 2;
#if 0
	gasf_memcpy(&obufTmp[obufTmpByte], &mapingFamily, 1);
#else
	obufTmp[obufTmpByte] = 0;
#endif
	obufTmpByte += 1;
	packBufByte = obufTmpByte;
	ogg_packet op;
	op.packet = packBuf;
	op.bytes = packBufByte;
	op.b_o_s = 1;
	op.e_o_s = 0;
	op.granulepos = 0;
	op.packetno = 0;
	ogg_stream_packetin(&_oggStreamS, &op);
	ogg_page oggPage;
	ret = ogg_stream_pageout(&_oggStreamS, &oggPage);
	gasf_memcpy(_idPageCache, oggPage.header, oggPage.header_len);
	gasf_memcpy(_idPageCache + oggPage.header_len, oggPage.body, oggPage.body_len);
	_idPage.headData = (OggPageHead_t*)_idPageCache;
	_idPage.headLen = oggPage.header_len;
	_idPage.bodyData = _idPageCache + oggPage.header_len;
	_idPage.bodyLen = oggPage.body_len;

	//user comment head
	obufTmp = packBuf;
	obufTmpByte = 0;
	if (_param.mode == Str2Key("opus"))
		gasf_memcpy(&obufTmp[obufTmpByte], "OpusTags", 8);
	else
		gasf_memcpy(&obufTmp[obufTmpByte], "unknowns", 8);
	obufTmpByte += 8;
	gasf_memcpy(&obufTmp[obufTmpByte], &_param.userComment.vendorStringLen, 4);
	obufTmpByte += 4;
	gasf_memcpy(&obufTmp[obufTmpByte], _param.userComment.vendorString, _param.userComment.vendorStringLen);
	obufTmpByte += _param.userComment.vendorStringLen;

	uint32_t userCommentListLength = 1;
	gasf_memcpy(&obufTmp[obufTmpByte], &userCommentListLength, 4);
	obufTmpByte += 4;

	gasf_memcpy(&obufTmp[obufTmpByte], &_param.userComment.userCommentStringLen, 4);
	obufTmpByte += 4;

	gasf_memcpy(&obufTmp[obufTmpByte], _param.userComment.userCommentString, _param.userComment.userCommentStringLen);
	obufTmpByte += _param.userComment.userCommentStringLen;
	packBufByte = obufTmpByte;
	
	op.packet = packBuf;
	op.bytes = packBufByte;
	op.b_o_s = 0;
	op.e_o_s = 0;
	op.granulepos = 0;
	op.packetno = 0;
	ogg_stream_packetin(&_oggStreamS, &op);
	ret = ogg_stream_flush_fill(&_oggStreamS, &oggPage, 1);
	if (ret == 0) {
		return OGG_RET_SUCCESS;
	}
	gasf_memcpy(_userCommentPageCache, oggPage.header, oggPage.header_len);
	gasf_memcpy(_userCommentPageCache + oggPage.header_len, oggPage.body, oggPage.body_len);
	_userCommentPage.headData = (OggPageHead_t*)_userCommentPageCache;
	_userCommentPage.headLen = oggPage.header_len;
	_userCommentPage.bodyData = _userCommentPageCache + oggPage.header_len;
	_userCommentPage.bodyLen = oggPage.body_len;
	_stage = Stage_e::STAGE_ID_HEAD;

#if 0
	if (ret != OGG_RET_SUCCESS) {
		LOG_OGG("open fail");
		Close();
		return ret;
	}
#endif
	return OGG_RET_SUCCESS;
}
OggRet_t OggMuxer_c::Set(uint32_t key, void* val) {
#if 1
	switch (key)
	{
	case Str2Key("mode"): _param.mode = Str2Key((const char*)val); return OGG_RET_SUCCESS;
	case Str2Key("version"): _param.idParam.version = (uint8_t)(uint32_t)val; return OGG_RET_SUCCESS;
	case Str2Key("ch"): _param.idParam.channel = (uint8_t)(uint32_t)val; return OGG_RET_SUCCESS;
	case Str2Key("preSkip"): _param.idParam.preSkip = (uint16_t)(uint32_t)val; return OGG_RET_SUCCESS;
	case Str2Key("fs"): _param.idParam.sampleRate = (uint32_t)val; return OGG_RET_SUCCESS;
	case Str2Key("oGain"): _param.idParam.outPutGain = (uint16_t)(uint32_t)val; return OGG_RET_SUCCESS;
	case Str2Key("vendor"):
		_param.userComment.vendorString = (const char*)(uint32_t)val;
		_param.userComment.vendorStringLen = strlen(_param.userComment.vendorString);
		return OGG_RET_SUCCESS;
	case Str2Key("comment"):
		_param.userComment.userCommentString = (const char*)(uint32_t)val;
		_param.userComment.userCommentStringLen = strlen(_param.userComment.userCommentString);
		return OGG_RET_SUCCESS;
	case Str2Key("pageByte"):_param.page_byte_round = (uint32_t)val; return OGG_RET_SUCCESS;
	case Str2Key("gPos"):_granulepos = (uint32_t)val; return OGG_RET_SUCCESS;
	case Str2Key("eos"):
		_isEos = (bool)(uint32_t)val;
		_oggStreamS.e_o_s = _isEos;
		return OGG_RET_SUCCESS;
	default:break;
	}
#endif
	if (Base_c::Set(key, val) == GASF_RET_SUCCESS) return OGG_RET_SUCCESS;
	return OGG_RET_FAIL;
}
OggRet_t OggMuxer_c::Get(uint32_t key, void* val) {
#if 1
	switch (key)
	{
		//case Str2Key("test"): _param.mode = (OggMuxerApiMode_e)(uint32_t)val; return OGG_RET_SUCCESS;
	default:break;
	}
#endif
	if (Base_c::Get(key, val) == GASF_RET_SUCCESS) return OGG_RET_SUCCESS;
	return OGG_RET_FAIL;
}
#if 0
OggRet_t OggMuxer_c::Run(GasfData_c& iData, GasfData_c& oData) {
	return OGG_RET_FAIL;
}
#endif
OggRet_t OggMuxer_c::Receive(GasfData_c& iData) {
	if (_isReceiveEnd)
		return OGG_RET_SUCCESS;
	_packetno++;
	ogg_packet op;
	op.packet = (uint8_t*)iData.Data();
	op.bytes = iData.Size();
	op.b_o_s = 0;
	op.e_o_s = _isEos ? 1 : 0;
	_isReceiveEnd = _isEos ? 1 : 0;
	op.granulepos = _granulepos;
	op.packetno = _packetno;
	ogg_stream_packetin(&_oggStreamS, &op);
	iData.Used(iData.Size());
	return OGG_RET_SUCCESS;
}
OggRet_t OggMuxer_c::Generate(GasfData_c& oData) {
	OggPage_t page = { 0,0,0,0 };
	if (_isGenrateEnd)
		return OGG_RET_SUCCESS;
	if (_stage == Stage_e::STAGE_NONE) {
		return OGG_RET_FAIL;
	}
	_isGenrateEnd = _isReceiveEnd;

	if (_stage == Stage_e::STAGE_ID_HEAD) {
		page = _idPage;
		_stage = Stage_e::STAGE_USER_COMMENT_HEAD;
	}
	else if (_stage == Stage_e::STAGE_USER_COMMENT_HEAD) {
		page = _userCommentPage;
		_stage = Stage_e::STAGE_DATA_HEAD;
	}
	else if (_stage == Stage_e::STAGE_DATA_HEAD) {
		int32_t ret = ogg_stream_pageout_fill(&_oggStreamS, &_oggPage, _page_out_fill_byte);
		if (ret == 0) {
			return OGG_RET_MORE_DATA;
			//return OGG_RET_SUCCESS;
		}
		page.headData = (OggPageHead_t*)_oggPage.header;
		page.headLen = _oggPage.header_len;
		page.bodyData = _oggPage.body;
		page.bodyLen = _oggPage.body_len;
	}
	else {
		return OGG_RET_FAIL;
	}
	if (oData.Flag() & (uint32_t)DataFlag_e::DATA_FLAG_GET_DATA_PAGE) {
		OggPage_t* ptr = (OggPage_t*)oData.LeftData();
		*ptr = page;
		oData.Append(sizeof(OggPage_t));
	}
	else {
		if (oData.LeftSize() < page.headLen + page.bodyLen) {
			return OGG_RET_FAIL;
		}
		oData.Append(page.headData, page.headLen);
		oData.Append(page.bodyData, page.bodyLen);
	}
	return OGG_RET_SUCCESS;
}
OggRet_t OggMuxer_c::Close() {
	ogg_stream_clear(&_oggStreamS);
	return OGG_RET_SUCCESS;
}

