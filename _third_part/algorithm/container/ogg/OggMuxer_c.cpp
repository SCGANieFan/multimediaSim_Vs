#include "OggMuxer_c.h"
using namespace ogg_ns;
OggRet_t OggMuxer_c::Init(OggMuxerApiParam_t* param, OggBasePorting_t *basePorting) {
	_MM.Init((OggBasePorting_t*)basePorting);
	ogg_sync_init(&_oggSyncS, &_MM);
	ogg_stream_init(&_oggStreamS, 0x34bd01b1, &_MM);
	if (param->page_byte_round) _page_out_fill_byte = param->page_byte_round;
	uint8_t packBuf[512];
	int32_t packBufByte = 0;
#if 1
	//id head
	uint8_t* obufTmp = packBuf;
	uint32_t obufTmpByte = 0;
	if (param->mode == OggMuxerApiMode_e::OGG_MUXER_API_MODE_OPUS) {
		memcpy(&obufTmp[obufTmpByte], "OpusHead", 8);
	}
	else {
		memcpy(&obufTmp[obufTmpByte], "unknowns", 8);
	}
	obufTmpByte += 8;
	memcpy(&obufTmp[obufTmpByte], &param->idParam.version, 1);
	obufTmpByte += 1;
	memcpy(&obufTmp[obufTmpByte], &param->idParam.channel, 1);
	obufTmpByte += 1;
	memcpy(&obufTmp[obufTmpByte], &param->idParam.preSkip, 2);
	obufTmpByte += 2;
	memcpy(&obufTmp[obufTmpByte], &param->idParam.sampleRate, 4);
	obufTmpByte += 4;
	memcpy(&obufTmp[obufTmpByte], &param->idParam.outPutGain, 2);
	obufTmpByte += 2;
#if 0
	memcpy(&obufTmp[obufTmpByte], &mapingFamily, 1); 
#else
	obufTmp[obufTmpByte] = 0;
#endif
	obufTmpByte += 1;
	packBufByte = obufTmpByte;
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
	obufTmpByte = 0;
	if(param->mode== OggMuxerApiMode_e::OGG_MUXER_API_MODE_OPUS)
		memcpy(&obufTmp[obufTmpByte], "OpusTags", 8);
	else
		memcpy(&obufTmp[obufTmpByte], "unknowns", 8);
	obufTmpByte += 8;
	memcpy(&obufTmp[obufTmpByte], &param->userComment.vendorStringLen, 4);
	obufTmpByte += 4;
	memcpy(&obufTmp[obufTmpByte], param->userComment.vendorString, param->userComment.vendorStringLen);
	obufTmpByte += param->userComment.vendorStringLen;

	uint32_t userCommentListLength = 1;
	memcpy(&obufTmp[obufTmpByte], &userCommentListLength, 4);
	obufTmpByte += 4;

	memcpy(&obufTmp[obufTmpByte], &param->userComment.userCommentStringLen, 4);
	obufTmpByte += 4;

	memcpy(&obufTmp[obufTmpByte], param->userComment.userCommentString, param->userComment.userCommentStringLen);
	obufTmpByte += param->userComment.userCommentStringLen;
	packBufByte = obufTmpByte;
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
OggRet_t OggMuxer_c::Receive(uint8_t* buf, int32_t len) {
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
OggRet_t OggMuxer_c::Generate(OggPage_t *page) {
#if 0
	if (_isGenrateEnd)
		return OGG_RET_FINISH;

	if (_stage == Stage_e::STAGE_ID_HEAD
		|| _stage == Stage_e::STAGE_USER_COMMENT_HEAD) {
		return OGG_RET_SUCCESS;
	}

	_isGenrateEnd = _isReceiveEnd;
	//int32_t ret = ogg_stream_pageout(&_oggStreamS, &_oggPage);
	int32_t ret = ogg_stream_pageout_fill(&_oggStreamS, &_oggPage, _page_out_fill_byte);
	if (ret == 0) {
		return OGG_RET_MORE_DATA;
	}
	return OGG_RET_SUCCESS;
#else
	memset(page, 0, sizeof(OggPage_t));
	if (_isGenrateEnd)
		return OGG_RET_FINISH;
	_isGenrateEnd = _isReceiveEnd;

	if (_stage == Stage_e::STAGE_ID_HEAD) {
		*page = _idPage;
		_stage = Stage_e::STAGE_USER_COMMENT_HEAD;
		return OGG_RET_SUCCESS;
	}
	else if (_stage == Stage_e::STAGE_USER_COMMENT_HEAD) {
		*page = _userCommentPage;
		_stage = Stage_e::STAGE_DATA_HEAD;
		return OGG_RET_SUCCESS;
	}
	else if (_stage == Stage_e::STAGE_DATA_HEAD) {
		//int32_t ret = ogg_stream_pageout(&_oggStreamS, &_oggPage);
		int32_t ret = ogg_stream_pageout_fill(&_oggStreamS, &_oggPage, _page_out_fill_byte);
		if (ret == 0) {
			return OGG_RET_MORE_DATA;
		}
		page->headData = (OggPageHead_t*)_oggPage.header;
		page->headLen = _oggPage.header_len;
		page->bodyData = _oggPage.body;
		page->bodyLen = _oggPage.body_len;
	}
	return OGG_RET_SUCCESS;
#endif
}
OggRet_t OggMuxer_c::Set(OggMuxerApiSet_e choose, void* val) {
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
OggRet_t OggMuxer_c::Get(OggMuxerApiGet_e choose, void* val) {
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
OggRet_t OggMuxer_c::DeInit() {
	ogg_stream_clear(&_oggStreamS);
	_MM.FreeAll();
	return OGG_RET_SUCCESS;
}
