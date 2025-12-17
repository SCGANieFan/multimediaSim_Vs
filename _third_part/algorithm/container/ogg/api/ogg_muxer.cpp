#include <string.h>
#include "ogg_muxer.h"
using namespace ogg_api_ns;
OggRet_t OggMuxer_c::Open(){
	if (!_param) {
		return OGG_API_RET_FAIL;
	}
	ogg_memory_t memory;
	memory.malloc_cb = _bp.malloc_cb;
	memory.realloc_cb = _bp.realloc_cb;
	memory.free_cb = _bp.free_cb;
	ogg_sync_init(&_oggSyncS, &memory);
	ogg_stream_init(&_oggStreamS, 0x34bd01b1, &memory);
	if (_param->page_byte_round) _page_out_fill_byte = _param->page_byte_round;
	uint8_t packBuf[512];
	int32_t packBufByte = 0;
#if 1
	//id head
	uint8_t* obufTmp = packBuf;
	uint32_t obufTmpByte = 0;
	if (_param->mode == Str2Key("opus")) {
		memcpy(&obufTmp[obufTmpByte], "OpusHead", 8);
	}
	else {
		memcpy(&obufTmp[obufTmpByte], "unknowns", 8);
	}
	obufTmpByte += 8;
	memcpy(&obufTmp[obufTmpByte], &_param->idParam.version, 1);
	obufTmpByte += 1;
	memcpy(&obufTmp[obufTmpByte], &_param->idParam.channel, 1);
	obufTmpByte += 1;
	memcpy(&obufTmp[obufTmpByte], &_param->idParam.preSkip, 2);
	obufTmpByte += 2;
	memcpy(&obufTmp[obufTmpByte], &_param->idParam.sampleRate, 4);
	obufTmpByte += 4;
	memcpy(&obufTmp[obufTmpByte], &_param->idParam.outPutGain, 2);
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
#if 0
	if (ret == 0) {
		return OGG_RET_MORE_DATA;
	}
#endif
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
	if (_param->mode == Str2Key("opus"))
		memcpy(&obufTmp[obufTmpByte], "OpusTags", 8);
	else
		memcpy(&obufTmp[obufTmpByte], "unknowns", 8);
	obufTmpByte += 8;
	memcpy(&obufTmp[obufTmpByte], &_param->userComment.vendorStringLen, 4);
	obufTmpByte += 4;
	memcpy(&obufTmp[obufTmpByte], _param->userComment.vendorString, _param->userComment.vendorStringLen);
	obufTmpByte += _param->userComment.vendorStringLen;

	uint32_t userCommentListLength = 1;
	memcpy(&obufTmp[obufTmpByte], &userCommentListLength, 4);
	obufTmpByte += 4;

	memcpy(&obufTmp[obufTmpByte], &_param->userComment.userCommentStringLen, 4);
	obufTmpByte += 4;

	memcpy(&obufTmp[obufTmpByte], _param->userComment.userCommentString, _param->userComment.userCommentStringLen);
	obufTmpByte += _param->userComment.userCommentStringLen;
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
		return OGG_API_RET_SUCCESS;
	}

	memcpy(_userCommentPageCache, oggPage.header, oggPage.header_len);
	memcpy(_userCommentPageCache + oggPage.header_len, oggPage.body, oggPage.body_len);
	_userCommentPage.headData = (OggPageHead_t*)_userCommentPageCache;
	_userCommentPage.headLen = oggPage.header_len;
	_userCommentPage.bodyData = _userCommentPageCache + oggPage.header_len;
	_userCommentPage.bodyLen = oggPage.body_len;
#endif
	_stage = Stage_e::STAGE_ID_HEAD;
	return OGG_API_RET_SUCCESS;
}
OggRet_t OggMuxer_c::Set(uint32_t key, void* val){
	switch (key)
	{
	case Str2Key("param"):_param = (OggMuxerApiParam_t*)val; return OGG_API_RET_SUCCESS;
	case Str2Key("gPos"):
		_granulepos = (uint32_t)val;
		return OGG_API_RET_SUCCESS;
	case Str2Key("eos"):
		_isEos = (bool)(uint32_t)val;
		_oggStreamS.e_o_s = _isEos;
		return OGG_API_RET_SUCCESS;
		
	default:break;
	}
	return Base_c::Set(key, val);
}
OggRet_t OggMuxer_c::Get(uint32_t key, void* val){
	switch (key)
	{
	//case Str2Key("test"): _param.mode = (OggMuxerApiMode_e)(uint32_t)val; return OGG_API_RET_SUCCESS;
	default:break;
	}
	return Base_c::Get(key, val);
}
#if 0
OggRet_t OggMuxer_c::Run(GaapiData_c& iData, GaapiData_c& oData){

}
#endif
OggRet_t OggMuxer_c::Receive(GaapiData_c& iData){
	if (_isReceiveEnd)
		return OGG_API_RET_SUCCESS;
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
	return OGG_API_RET_SUCCESS;
}
OggRet_t OggMuxer_c::Generate(GaapiData_c& oData){
	OggPage_t page;
	if (_isGenrateEnd)
		return OGG_API_RET_SUCCESS;
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
			return OGG_API_RET_SUCCESS;
		}
		page.headData = (OggPageHead_t*)_oggPage.header;
		page.headLen = _oggPage.header_len;
		page.bodyData = _oggPage.body;
		page.bodyLen = _oggPage.body_len;
	}
	if (oData.LeftSize() < page.headLen + page.bodyLen) {
		return OGG_API_RET_FAIL;
	}
	oData.Append(page.headData, page.headLen);
	oData.Append(page.bodyData, page.bodyLen);
	return OGG_API_RET_SUCCESS;
}
OggRet_t OggMuxer_c::Close(){
	ogg_stream_clear(&_oggStreamS);
	return OGG_API_RET_SUCCESS;
}

EXTERNC void ogg_api_register_ogg_muxer() {
	gaapi_gaf_register<OggMuxer_c>("oggMuxer");
}
