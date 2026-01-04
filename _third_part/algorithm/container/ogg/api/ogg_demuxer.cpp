#include <string.h>
#include "ogg_demuxer.h"
using namespace ogg_api_ns;
using namespace ogg_gaapi_ns;
OggDeMuxer_c::OggDeMuxer_c() {
}
OggDeMuxer_c::~OggDeMuxer_c() {
}

OggRet_t OggDeMuxer_c::Open(){
	_memory.malloc_cb = _bp.malloc_cb;
	_memory.realloc_cb = _bp.realloc_cb;
	_memory.free_cb = _bp.free_cb;
#if 0
	ogg_sync_init(&_oggSyncS, &_memory);
	memset(&_oggPacketOld, 0, sizeof(ogg_packet));
	_oggPacketOld.packet = _oBufCache;
	_receiveInfo.bufMax = 2 * 1024;
#endif
	uint32_t bufMax = 4 * 1024;
	void* buf = _memory.malloc_cb(bufMax);
	if (!buf) return OGG_API_RET_FAIL;
	_iCache.Init(buf, bufMax);
	return OGG_API_RET_SUCCESS;
}
OggRet_t OggDeMuxer_c::Set(uint32_t key, void* val){
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
	return Base_c::Set(key, val);
}
OggRet_t OggDeMuxer_c::Get(uint32_t key, void* val){
	switch (key)
	{
#if 0
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
		if (_stage > Stage_e::STAGE_ID_PAGE_HEAD)
			*(uint32_t*)val = 1;
		else
			*(uint32_t*)val = 0;
		return OGG_API_RET_SUCCESS;
	}
	case Str2Key("idPag"):
		*((OggPage_t**)val) = &_idPage;
		return OGG_API_RET_SUCCESS;
	case Str2Key("hUsrPage"):
		if (_stage > Stage_e::STAGE_USER_COMMENT_BODY)
			*(uint32_t*)val = 1;
		else
			*(uint32_t*)val = 0;
		return OGG_API_RET_SUCCESS;
	case Str2Key("usrPage"):
		*((OggPage_t**)val) = &_userPage;
		return OGG_API_RET_SUCCESS;
#endif
	case Str2Key("rate"):
		if (_stage < Stage_e::STAGE_USER_COMMENT_HEAD)return OGG_API_RET_FAIL;
		*(uint32_t*)val = _rate;
		return OGG_API_RET_SUCCESS;
	case Str2Key("ch"):
		if (_stage < Stage_e::STAGE_USER_COMMENT_HEAD)return OGG_API_RET_FAIL;
		*(uint32_t*)val = _ch;
		return OGG_API_RET_SUCCESS;
	default:break;
	}
	return Base_c::Get(key, val);
}
OggRet_t OggDeMuxer_c::Receive(GaapiData_c& iData) {
#if 0
	{
		
		static uint32_t cnt = 0;
		++cnt;
		//LOG_OGG("[%d]", cnt);
		if (cnt == 31) {
			uint8_t* bufSync = SyncString((uint8_t*)iData.Data(), iData.Size(), "OggS");
			uint32_t syncOffset = iData.Size() - 4 + 1;
			if (bufSync) syncOffset = bufSync - (uint8_t*)iData.Data();
			int a = 1;
		}


	}
#endif
	_iCache.ClearUsed();
	uint32_t appendByte = _iCache.Append(iData.Data(), iData.Size());
	iData.Used(appendByte);
	int32_t len = _iCache.Size();
	return OGG_API_RET_SUCCESS;
}
OggRet_t OggDeMuxer_c::Generate(GaapiData_c& oData){
	OggRet_t ret = OGG_API_RET_SUCCESS;
	switch (_stage) {
	case Stage_e::STAGE_ID_PAGE_HEAD:
		ret = DemuxIdPageHead(oData); if (ret != OGG_API_RET_SUCCESS)return ret;
	case Stage_e::STAGE_ID_PAGE_BODY:
		ret = DemuxIdPageBody(oData); if (ret != OGG_API_RET_SUCCESS)return ret;
	case Stage_e::STAGE_USER_COMMENT_HEAD:
		ret = DemuxUserCommentHead(oData); if (ret != OGG_API_RET_SUCCESS)return ret;
	case Stage_e::STAGE_USER_COMMENT_BODY:
		ret = DemuxUserCommentBody(oData); if (ret != OGG_API_RET_SUCCESS)return ret;
	case Stage_e::STAGE_DATA_HEAD: 
		ret = DemuxDataHead(oData); if (ret != OGG_API_RET_SUCCESS)return ret;
	case Stage_e::STAGE_DATA_BODY: return DemuxDataBody(oData);
	case Stage_e::STAGE_EOS: return DemuxEos(oData);
	default:break;
	}
	return OGG_API_RET_SUCCESS;
}

OggRet_t OggDeMuxer_c::Close() {
#if 0
	ogg_sync_clear(&_oggSyncS);
	ogg_stream_clear(&_oggStreamS);
#endif
	if (_iCache.Buf()) _memory.free_cb(_iCache.Buf());
	
	return OGG_API_RET_SUCCESS;
}

uint8_t* OggDeMuxer_c::SyncString(uint8_t* buf, uint32_t len, const char* str){
	uint32_t strLen = strlen(str);
	if (len < strLen) return 0;
	uint32_t off = 0;
	uint32_t validLen = len - strLen + 1;

	//sync first str
	while (off < validLen) {
		while (off < validLen) {
			if (buf[off] == str[0])
				break;
			++off;
		}
		if (off == validLen) {
			return 0;
		}
		uint16_t n = 0;
		for (n = 0; n < strLen; n++) {
			if (buf[off + n] != str[n])
				break;
		}
		if (n == strLen) {
			break;
		}
		++off;
	}
	if (off == validLen) return 0;
	return &buf[off];
}

OggRet_t OggDeMuxer_c::DemuxIdPageHead(GaapiData_c& oData){
	const char* syncStr = "OggS";
	uint32_t syncStrLen = strlen(syncStr);
	int32_t len = _iCache.Size();

	if (len < syncStrLen) return OGG_API_RET_MORE_DATA;
	uint8_t* bufSync = SyncString((uint8_t*)_iCache.Data(), len, syncStr);
	uint32_t syncOffset = len - syncStrLen + 1;
	if (bufSync) syncOffset = bufSync - (uint8_t*)_iCache.Data();
	_iCache.Used(syncOffset);
	if(!bufSync)  return OGG_API_RET_MORE_DATA;

	if (_iCache.Size() < _headByteCom)return OGG_API_RET_MORE_DATA;
	ogg_gaapi_ns::gaapi_memcpy(&_idPage2.page.head, _iCache.Data(), _headByteCom);
	_idPage2.page.headLen = _headByteCom + _idPage2.page.head.segmentNumber;
	if (_iCache.Size() < (_idPage2.page.headLen)) return OGG_API_RET_MORE_DATA;
	ogg_gaapi_ns::gaapi_memcpy(&_idPage2.page.head, _iCache.Data(), _idPage2.page.headLen);
	_iCache.Used(_idPage2.page.headLen);
	_idPage2.page.bodyLen = 0;
	for (uint8_t n = 0; n < _idPage2.page.head.segmentNumber; n++) {
		_idPage2.page.bodyLen += _idPage2.page.head.segment[n];
	}
	_idPage2.bodyLenRem = _idPage2.page.bodyLen;
	_idPage2.page.body = (uint8_t*) & _idPage2.idOpusBody;
	LOG_OGG("%d,%d", _idPage2.page.headLen, _idPage2.page.bodyLen);
	_stage = Stage_e::STAGE_ID_PAGE_BODY;
	return OGG_API_RET_SUCCESS;
}

OggRet_t OggDeMuxer_c::DemuxIdPageBody(GaapiData_c& oData) {
	uint32_t copyByte = _iCache.Size();
	copyByte = copyByte > _idPage2.bodyLenRem ? _idPage2.bodyLenRem : copyByte;
	ogg_gaapi_ns::gaapi_memcpy(_idPage2.page.body, _iCache.Data(), copyByte);
	_iCache.Used(copyByte);
	_idPage2.bodyLenRem -= copyByte;
	if (_idPage2.bodyLenRem) return OGG_API_RET_MORE_DATA;

	uint8_t* bufSync = 0;
	if (SyncString((uint8_t*)_idPage2.page.body, _idPage2.page.bodyLen, "vorbis")) {
		_idPage2.codecType = OggCodecType_e::OGG_CODEC_TYPE_VORBIS;
		_ch = _idPage2.idVorbisBody.channel;
		_rate = ((uint32_t)_idPage2.idVorbisBody.sampleRate[0])
			| (((uint32_t)_idPage2.idVorbisBody.sampleRate[1]) << 8)
			| (((uint32_t)_idPage2.idVorbisBody.sampleRate[2]) << 16)
			| (((uint32_t)_idPage2.idVorbisBody.sampleRate[3]) << 24)
			;
	}
	else if (SyncString((uint8_t*)_idPage2.page.body, _idPage2.page.bodyLen, "OpusHead")) {
		_idPage2.codecType = OggCodecType_e::OGG_CODEC_TYPE_OPUS;
		_ch = _idPage2.idOpusBody.channel;
		_rate = ((uint32_t)_idPage2.idOpusBody.sampleRate[0])
			| (((uint32_t)_idPage2.idOpusBody.sampleRate[1]) << 8)
			| (((uint32_t)_idPage2.idOpusBody.sampleRate[2]) << 16)
			| (((uint32_t)_idPage2.idOpusBody.sampleRate[3]) << 24)
			;
	}
	else {
		return OGG_API_RET_FAIL;
	}
	LOG_OGG("%d hz, %d ch, codec, %8s", _rate, _ch, _idPage2.page.body);
	_stage = Stage_e::STAGE_USER_COMMENT_HEAD;
	return OGG_API_RET_SUCCESS;
}

OggRet_t OggDeMuxer_c::DemuxUserCommentHead(GaapiData_c& oData){
	const char* syncStr = "OggS";
	uint32_t syncStrLen = strlen(syncStr);
	int32_t len = _iCache.Size();

	if (len < syncStrLen) return OGG_API_RET_MORE_DATA;
	uint8_t* bufSync = SyncString((uint8_t*)_iCache.Data(), len, syncStr);
	uint32_t syncOffset = len - syncStrLen + 1;
	if (bufSync) syncOffset = bufSync - (uint8_t*)_iCache.Data();
	_iCache.Used(syncOffset);
	if (!bufSync)  return OGG_API_RET_MORE_DATA;

	if (_iCache.Size() < _headByteCom)return OGG_API_RET_MORE_DATA;
	ogg_gaapi_ns::gaapi_memcpy(&_userPage2.page.head, _iCache.Data(), _headByteCom);
	_userPage2.page.headLen = _headByteCom + _userPage2.page.head.segmentNumber;
	if (_iCache.Size() < (_userPage2.page.headLen)) return OGG_API_RET_MORE_DATA;
	ogg_gaapi_ns::gaapi_memcpy(&_userPage2.page.head, _iCache.Data(), _userPage2.page.headLen);
	_iCache.Used(_userPage2.page.headLen);
	_userPage2.page.bodyLen = 0;
	for (uint8_t n = 0; n < _userPage2.page.head.segmentNumber; n++) {
		_userPage2.page.bodyLen += _userPage2.page.head.segment[n];
	}
	_userPage2.bodyLenRem = _userPage2.page.bodyLen;
	_userPage2.page.body = (uint8_t*)&_userPage2.userBody;
	LOG_OGG("%d,%d", _userPage2.page.headLen, _userPage2.page.bodyLen);
	_stage = Stage_e::STAGE_USER_COMMENT_BODY;
	return OGG_API_RET_SUCCESS;
}
OggRet_t OggDeMuxer_c::DemuxUserCommentBody(GaapiData_c& oData){
#if 0
	uint32_t copyByte = _iCache.Size();
	copyByte = copyByte > _userPage2.bodyLenRem ? _userPage2.bodyLenRem : copyByte;
	ogg_gaapi_ns::gaapi_memcpy(_userPage2.page.body, _iCache.Data(), copyByte);
	_iCache.Used(copyByte);
	_userPage2.bodyLenRem -= copyByte;
	if (_userPage2.bodyLenRem) return OGG_API_RET_MORE_DATA;
	uint8_t* bufSync = 0;
	if (SyncString((uint8_t*)_userPage2.page.body, _userPage2.page.bodyLen, "vorbis")) {
		_userPage2.codecType = OggCodecType_e::OGG_CODEC_TYPE_VORBIS;
	}
	else if (SyncString((uint8_t*)_userPage2.page.body, _userPage2.page.bodyLen, "OpusHead")) {
		_userPage2.codecType = OggCodecType_e::OGG_CODEC_TYPE_OPUS;
	}
	else {
		return OGG_API_RET_FAIL;
	}
	LOG_OGG("codec, %8s", _userPage2.page.body);
	_stage = Stage_e::STAGE_AUDIO_DATA;
	return OGG_API_RET_SUCCESS;
#else
	uint32_t usedByte = _iCache.Used(_userPage2.bodyLenRem);
	_userPage2.bodyLenRem -= usedByte;
	if (_userPage2.bodyLenRem) return OGG_API_RET_MORE_DATA;
	//LOG_OGG("codec, %8s", _userPage2.page.body);
	_stage = Stage_e::STAGE_DATA_HEAD;
	return OGG_API_RET_SUCCESS;
#endif

}
OggRet_t OggDeMuxer_c::DemuxDataHead(GaapiData_c& oData){
	//OggDataPage_t _dataPage2;
	const char* syncStr = "OggS";
	uint32_t syncStrLen = strlen(syncStr);
	int32_t len = _iCache.Size();

	if (len < syncStrLen) return OGG_API_RET_MORE_DATA;
	uint8_t* bufSync = SyncString((uint8_t*)_iCache.Data(), len, syncStr);
	uint32_t syncOffset = len - syncStrLen + 1;
	if (bufSync) syncOffset = bufSync - (uint8_t*)_iCache.Data();
	_iCache.Used(syncOffset);
	if (!bufSync)  return OGG_API_RET_MORE_DATA;

	if (_iCache.Size() < _headByteCom)return OGG_API_RET_MORE_DATA;
	ogg_gaapi_ns::gaapi_memcpy(&_dataPage2.page.head, _iCache.Data(), _headByteCom);
	_dataPage2.page.headLen = _headByteCom + _dataPage2.page.head.segmentNumber;
	if (_iCache.Size() < (_dataPage2.page.headLen)) return OGG_API_RET_MORE_DATA;
	ogg_gaapi_ns::gaapi_memcpy(&_dataPage2.page.head, _iCache.Data(), _dataPage2.page.headLen);
	_iCache.Used(_dataPage2.page.headLen);
	_dataPage2.page.bodyLen = 0;
	for (uint8_t n = 0; n < _dataPage2.page.head.segmentNumber; n++) {
		_dataPage2.page.bodyLen += _dataPage2.page.head.segment[n];
	}
	_dataPage2.bodyLenRem = _dataPage2.page.bodyLen;
	_dataPage2.packByte = 0;
	_dataPage2.packIndex = 0;
	_dataPage2.segmentIndex = -1;
#if 0
	{
		static uint32_t cnt = 0;
		cnt++;
		LOG_OGG("[%d] %d,%d,%d", cnt, syncOffset, _dataPage2.page.headLen, _dataPage2.page.bodyLen);
		if (cnt == 25)
			int a = 1;
	}
#endif
	_stage = Stage_e::STAGE_DATA_BODY;
	return OGG_API_RET_SUCCESS;
}

OggRet_t OggDeMuxer_c::DemuxDataBody(GaapiData_c& oData) {
	if (!_dataPage2.packByte) {
		uint32_t packByte = 0;
		uint8_t index = _dataPage2.segmentIndex + 1;
		uint8_t indexMax = _dataPage2.page.head.segmentNumber;
		uint8_t* segment = _dataPage2.page.head.segment;
		for (; index < indexMax; index++) {
			uint8_t seg = segment[index];
			packByte += seg;
			if (seg != 0xff)
				break;
		}
		_dataPage2.packByte = packByte;
		_dataPage2.segmentIndex = index;
		_dataPage2.packIndex++;
	}
	if (oData.LeftSize() < _dataPage2.packByte)return OGG_API_RET_OUT_BUFF_NOT_ENOUGH;
	if (_iCache.Size() < _dataPage2.packByte)return OGG_API_RET_MORE_DATA;
	oData.Append(_iCache.Data(), _dataPage2.packByte);
	_iCache.Used(_dataPage2.packByte); 
#if 0
	{
		static uint32_t cnt = 0;
		static uint32_t packByteAac = 0;
		cnt++;
		packByteAac += _dataPage2.packByte;
		if(cnt > 170)
			//LOG_OGG("[%d]%d,%d,(%x,%x)", cnt,_dataPage2.packByte, packByteAac, ((uint32_t*)oData.Data())[0], ((uint32_t*)oData.Data())[1]);
		if (_dataPage2.packByte == 0)
			int a = 1;
		if (cnt == 181) {
			uint32_t size = 0;
			for (uint8_t n = 0; n < _dataPage2.page.head.segmentNumber; n++) {
				size += _dataPage2.page.head.segment[n];
			}
			int a = 1;
		}
	}
#endif
	_dataPage2.packByte = 0;
	if (_dataPage2.segmentIndex >= (_dataPage2.page.head.segmentNumber - 1))
		_stage = Stage_e::STAGE_DATA_HEAD;
	if (_dataPage2.page.head.segment[_dataPage2.page.head.segmentNumber - 1] == 0xff)
		return OGG_API_RET_INCOMPLETE;
	return OGG_API_RET_SUCCESS;
}



OggRet_t OggDeMuxer_c::DemuxEos(GaapiData_c& oData){
	return OGG_API_RET_SUCCESS;
}


EXTERNC void ogg_api_register_ogg_demuxer() {
	gaapi_gaf_register<OggDeMuxer_c>("oggDemux");
}
