#include "ogg_demuxer.h"

using namespace GASF_NAME_SPACE;
using namespace ogg_ns;
OggDemuxer_c::OggDemuxer_c() {
}
OggDemuxer_c::~OggDemuxer_c() {
}
OggRet_t OggDemuxer_c::Open() {
	LOG_OGG("v%s", OGG_VERSION);
	if (!_bp.malloc_cb
		|| !_bp.realloc_cb
		|| !_bp.free_cb) {
		return OGG_RET_FAIL;
	}
	_memory.malloc_cb = _bp.malloc_cb;
	_memory.realloc_cb = _bp.realloc_cb;
	_memory.free_cb = _bp.free_cb;
	uint32_t bufMax = 4 * 1024;
	gasf_memset(&_userPage, 0, sizeof(_userPage));
	void* buf = _memory.malloc_cb(bufMax);
	if (!buf) return OGG_RET_FAIL;
	_iCache.Init(buf, bufMax);
#if 0
	OggRet_t ret;
	if (ret != OGG_RET_SUCCESS) {
		LOG_OGG("open fail");
		Close();
		return ret;
	}
#endif
	return OGG_RET_SUCCESS;
}
OggRet_t OggDemuxer_c::Set(uint32_t key, void* val) {
	switch (key)
	{
	case Str2Key("reset"):
		_stage = Stage_e::STAGE_ID_PAGE_HEAD;
		_iCache.Used(_iCache.Size());
		_iCache.ClearUsed();
		return OGG_RET_SUCCESS;
	case Str2Key("resetToData"):
		_stage = Stage_e::STAGE_DATA_FIRST_HEAD;
		_iCache.Used(_iCache.Size());
		_iCache.ClearUsed();
		return OGG_RET_SUCCESS;
	default:break;
	}
	if (OggBase_c::Set(key, val) == GASF_RET_SUCCESS) return OGG_RET_SUCCESS;
	return OGG_RET_FAIL;
}
OggRet_t OggDemuxer_c::Get(uint32_t key, void* val) {
	switch (key)
	{
	case Str2Key("rate"):
		if (_stage < Stage_e::STAGE_USER_COMMENT_HEAD)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _rate;
		return OGG_RET_SUCCESS;
	case Str2Key("ch"):
		if (_stage < Stage_e::STAGE_USER_COMMENT_HEAD)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _ch;
		return OGG_RET_SUCCESS;
	case Str2Key("granulePos"):
	{
		if (_stage < Stage_e::STAGE_DATA_HEAD)return OGG_RET_FAIL;
		uint64_t granulePos = 0;
		for (int8_t n = 7; n >= 0; n--) {
			granulePos = (granulePos << 8) | _dataPage.page.head.granulePos[n];
		}
		if (val) *(uint64_t*)val = granulePos;
		return OGG_RET_SUCCESS;
	}
	case Str2Key("idPageLen"):
		if (_stage < Stage_e::STAGE_ID_PAGE_BODY)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _idPage.page.headLen + _idPage.page.bodyLen;
		return OGG_RET_SUCCESS;
	case Str2Key("idPageBodyLen"):
		if (_stage < Stage_e::STAGE_ID_PAGE_BODY)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _idPage.page.bodyLen;
		return OGG_RET_SUCCESS;
	case Str2Key("idPageBody"):
		if (_stage < Stage_e::STAGE_USER_COMMENT_HEAD)return OGG_RET_FAIL;
		if (val) *(uint8_t**)val = _idPage.page.body;
		return OGG_RET_SUCCESS;
	case Str2Key("userPageLen"):
		if (_stage < Stage_e::STAGE_USER_COMMENT_BODY)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _userPage.page.headLen + _userPage.page.bodyLen;
		return OGG_RET_SUCCESS;
	case Str2Key("userPageBodyLen"):
		if (_stage < Stage_e::STAGE_USER_COMMENT_BODY)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _userPage.page.bodyLen;
		return OGG_RET_SUCCESS;
	case Str2Key("userPagePacketNum"):
		if (_stage < Stage_e::STAGE_DATA_FIRST_HEAD)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _userPage.packetNum;
		return OGG_RET_SUCCESS;
	case Str2Key("userPagePacket"):
		if (_stage < Stage_e::STAGE_DATA_FIRST_HEAD)return OGG_RET_FAIL;
		if (val) *(uint8_t***)val = _userPage.packet;
		return OGG_RET_SUCCESS;
	case Str2Key("userPagePacketLen"):
		if (_stage < Stage_e::STAGE_DATA_FIRST_HEAD)return OGG_RET_FAIL;
		if (val) *(uint32_t**)val = _userPage.packetLen;
		return OGG_RET_SUCCESS;
	case Str2Key("title"):
		if (_stage < Stage_e::STAGE_DATA_FIRST_HEAD)return OGG_RET_FAIL;
		if (!_userPage._titleLen)return OGG_RET_FAIL;
		if (val) *(uint8_t**)val = _userPage._title;
		return OGG_RET_SUCCESS;
	case Str2Key("titleLen"):
		if (_stage < Stage_e::STAGE_DATA_FIRST_HEAD)return OGG_RET_FAIL;
		if (!_userPage._titleLen)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _userPage._titleLen;
		return OGG_RET_SUCCESS;
	case Str2Key("artist"):
		if (_stage < Stage_e::STAGE_DATA_FIRST_HEAD)return OGG_RET_FAIL;
		if (!_userPage._artistLen)return OGG_RET_FAIL;
		if (val) *(uint8_t**)val = _userPage._artist;
		return OGG_RET_SUCCESS;
	case Str2Key("artistLen"):
		if (_stage < Stage_e::STAGE_DATA_FIRST_HEAD)return OGG_RET_FAIL;
		if (!_userPage._artistLen)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _userPage._artistLen;
		return OGG_RET_SUCCESS;
	case Str2Key("album"):
		if (_stage < Stage_e::STAGE_DATA_FIRST_HEAD)return OGG_RET_FAIL;
		if (!_userPage._albumLen)return OGG_RET_FAIL;
		if (val) *(uint8_t**)val = _userPage._album;
		return OGG_RET_SUCCESS;
	case Str2Key("albumLen"):
		if (_stage < Stage_e::STAGE_DATA_FIRST_HEAD)return OGG_RET_FAIL;
		if (!_userPage._albumLen)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _userPage._albumLen;
		return OGG_RET_SUCCESS;
	case Str2Key("dataPageLen"):
		if (_stage < Stage_e::STAGE_DATA_HEAD)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _dataPage.page.headLen + _dataPage.page.bodyLen;
		return OGG_RET_SUCCESS;
	case Str2Key("dataPageNum"):
	{
		if (_stage < Stage_e::STAGE_DATA_HEAD)return OGG_RET_FAIL;
		uint32_t pageNum = 0;
		for (int8_t n = 3; n >= 0; n--) {
			pageNum = (pageNum << 8) | _dataPage.page.head.pageCounter[n];
		}
		if (val) *(uint32_t*)val = pageNum;
		return OGG_RET_SUCCESS;
	}
	case Str2Key("dataPacketNum"):
		if (_stage < Stage_e::STAGE_DATA_HEAD)return OGG_RET_FAIL;
		if (val) *(uint32_t*)val = _dataPage.packNum;
		return OGG_RET_SUCCESS;
	case Str2Key("stageHead"):
		if (_stage < Stage_e::STAGE_DATA_FIRST_HEAD)return OGG_RET_FAIL;
		return OGG_RET_SUCCESS;
	default:break;
	}
	if (OggBase_c::Get(key, val) == GASF_RET_SUCCESS) return OGG_RET_SUCCESS;
	return OGG_RET_FAIL;
}
OggRet_t OggDemuxer_c::Receive(OggData_c& iData) {
	_iCache.ClearUsed();
	uint32_t appendByte = _iCache.Append(iData.Data(), iData.Size());
	iData.Used(appendByte);
	int32_t len = _iCache.Size();
	return OGG_RET_SUCCESS;
}
OggRet_t OggDemuxer_c::Generate(OggData_c& oData) {
	OggRet_t ret = OGG_RET_SUCCESS;
	switch (_stage) {
	case Stage_e::STAGE_ID_PAGE_HEAD:
		return DemuxIdPageHead(oData);
	case Stage_e::STAGE_ID_PAGE_BODY:
		return DemuxIdPageBody(oData);
	case Stage_e::STAGE_USER_COMMENT_HEAD:
		return DemuxUserCommentHead(oData);
	case Stage_e::STAGE_USER_COMMENT_BODY:
		return DemuxUserCommentBody(oData);
	case Stage_e::STAGE_DATA_FIRST_HEAD:
	case Stage_e::STAGE_DATA_HEAD:
		return DemuxDataHead(oData);
	case Stage_e::STAGE_DATA_BODY: return DemuxDataBody(oData);
	case Stage_e::STAGE_EOS: return DemuxEos(oData);
	default:break;
	}
	return OGG_RET_SUCCESS;
}
OggRet_t OggDemuxer_c::Close() {
	if (_iCache.Buf()) _memory.free_cb(_iCache.Buf());
	if (_userPage.page.body) _memory.free_cb(_userPage.page.body);
	return OGG_RET_SUCCESS;
}


bool OggDemuxer_c::Str2Low(uint8_t* str, uint32_t strLen) {
	if (!strLen) strLen = 4096;
	uint32_t i = 0;
	for (; str[i] != '\0'; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] += 32;
		}
		if (i >= (strLen - 1)) break;
	}
	if (i >= strLen) return false;
	return true;
}

bool OggDemuxer_c::StrLen(uint8_t* str, uint32_t* strLen, uint32_t searchLenMax) {
	if (strLen) *strLen = 0;
	uint32_t len = 0;
	searchLenMax += 1;
	while ((len < searchLenMax) && (str[len] != '\0')) ++len;
	if (strLen) *strLen = len;
	if (len == searchLenMax) return false;
	return true;
}

bool OggDemuxer_c::SyncString(uint8_t* searchStr, uint32_t searchStrLen, uint8_t* syncStr, uint8_t** oStr, uint32_t* oStrLen) {
	uint32_t strLen = 0;
	bool ret = StrLen(syncStr, &strLen, searchStrLen);
	if (!ret) strLen = 0;
	return SyncString(searchStr, searchStrLen, syncStr, strLen, oStr, oStrLen);
}

bool OggDemuxer_c::SyncString(uint8_t* searchStr, uint32_t searchStrLen, uint8_t* syncStr, uint32_t syncStrLen, uint8_t** oStr, uint32_t* oStrLen) {
	if (oStr) *oStr = 0;
	if (oStrLen) *oStrLen = 0;
	if (searchStrLen < syncStrLen) return false;
	uint32_t off = 0;
	uint32_t searchValidLen = searchStrLen - syncStrLen + 1;
	while (off < searchValidLen) {
		while (off < searchValidLen) {
			if (searchStr[off] == syncStr[0])
				break;
			++off;
		}
		if (off == searchValidLen) break;
		uint16_t n = 0;
		for (n = 0; n < syncStrLen; n++) {
			if (searchStr[off + n] != syncStr[n])
				break;
		}
		if (n == syncStrLen) {
			break;
		}
		++off;
	}
	if (oStr) *oStr = &searchStr[off];
	if (oStrLen) *oStrLen = off;
	if (off == searchValidLen) return false;
	return true;
}

OggRet_t OggDemuxer_c::DemuxIdPageHead(OggData_c& oData) {
	int32_t len = _iCache.Size();
	if (len < _syncStrLen) return OGG_RET_MORE_DATA;
	uint32_t syncOffset = 0;
	bool ret = SyncString((uint8_t*)_iCache.Data(), len, (uint8_t*)_syncStr, _syncStrLen, 0, &syncOffset);
	_iCache.Used(syncOffset);
	if (!ret)  return OGG_RET_MORE_DATA;
	if (_iCache.Size() < _headByteCom)return OGG_RET_MORE_DATA;
	gasf_memset(&_idPage, 0, sizeof(_idPage));
	gasf_memcpy(&_idPage.page.head, _iCache.Data(), _headByteCom);
	_idPage.page.headLen = _headByteCom + _idPage.page.head.segmentNumber;
	if (_iCache.Size() < (_idPage.page.headLen)) return OGG_RET_MORE_DATA;
	gasf_memcpy(&_idPage.page.head, _iCache.Data(), _idPage.page.headLen);
	_iCache.Used(_idPage.page.headLen);

	_idPage.page.bodyLen = 0;
#if 1
	if (_idPage.page.head.segmentNumber != 1)
		LOG_OGG("warning, %d", _idPage.page.head.segmentNumber);
#endif
	for (uint8_t n = 0; n < _idPage.page.head.segmentNumber; n++) {
		_idPage.page.bodyLen += _idPage.page.head.segment[n];
	}
	_idPage.bodyLenRem = _idPage.page.bodyLen;
	_idPage.page.body = (uint8_t*)&_idPage.idOpusBody;
	LOG_OGG("%d,%d", _idPage.page.headLen, _idPage.page.bodyLen);
	_stage = Stage_e::STAGE_ID_PAGE_BODY;
	return OGG_RET_SUCCESS;
}

OggRet_t OggDemuxer_c::DemuxIdPageBody(OggData_c& oData) {
	uint32_t copyByte = _iCache.Size();
	copyByte = copyByte > _idPage.bodyLenRem ? _idPage.bodyLenRem : copyByte;
	gasf_memcpy(_idPage.page.body + (_idPage.page.bodyLen - _idPage.bodyLenRem), _iCache.Data(), copyByte);
	_iCache.Used(copyByte);
	_idPage.bodyLenRem -= copyByte;
	if (_idPage.bodyLenRem) return OGG_RET_MORE_DATA;

	if (SyncString((uint8_t*)_idPage.page.body, _idPage.page.bodyLen, (uint8_t*)"\x01vorbis")) {
		_idPage.codecType = OggCodecType_e::OGG_CODEC_TYPE_VORBIS;
		_ch = _idPage.idVorbisBody.channel;
		_rate = ((uint32_t)_idPage.idVorbisBody.sampleRate[0])
			| (((uint32_t)_idPage.idVorbisBody.sampleRate[1]) << 8)
			| (((uint32_t)_idPage.idVorbisBody.sampleRate[2]) << 16)
			| (((uint32_t)_idPage.idVorbisBody.sampleRate[3]) << 24)
			;
	}
	else if (SyncString((uint8_t*)_idPage.page.body, _idPage.page.bodyLen, (uint8_t*)"OpusHead")) {
		_idPage.codecType = OggCodecType_e::OGG_CODEC_TYPE_OPUS;
		_ch = _idPage.idOpusBody.channel;
		_rate = ((uint32_t)_idPage.idOpusBody.sampleRate[0])
			| (((uint32_t)_idPage.idOpusBody.sampleRate[1]) << 8)
			| (((uint32_t)_idPage.idOpusBody.sampleRate[2]) << 16)
			| (((uint32_t)_idPage.idOpusBody.sampleRate[3]) << 24)
			;
		_rate = 48000;
	}
	else {
		LOG_OGG("%32s", _idPage.page.body);
		return OGG_RET_FAIL;
	}
	LOG_OGG("%d hz, %d ch, codec: %08s", _rate, _ch, _idPage.page.body);
	_stage = Stage_e::STAGE_USER_COMMENT_HEAD;
	return OGG_RET_SUCCESS;
}

OggRet_t OggDemuxer_c::DemuxUserCommentHead(OggData_c& oData) {
	int32_t len = _iCache.Size();
	if (len < _syncStrLen) return OGG_RET_MORE_DATA;
	uint32_t syncOffset = 0;
	bool ret = SyncString((uint8_t*)_iCache.Data(), len, (uint8_t*)_syncStr, _syncStrLen, 0, &syncOffset);
	_iCache.Used(syncOffset);
	if (!ret)  return OGG_RET_MORE_DATA;
	if (_iCache.Size() < _headByteCom)return OGG_RET_MORE_DATA;
	gasf_memset(&_userPage, 0, sizeof(_userPage));
	gasf_memcpy(&_userPage.page.head, _iCache.Data(), _headByteCom);
	_userPage.page.headLen = _headByteCom + _userPage.page.head.segmentNumber;
	if (_iCache.Size() < (_userPage.page.headLen)) return OGG_RET_MORE_DATA;
	gasf_memcpy(&_userPage.page.head, _iCache.Data(), _userPage.page.headLen);
	_iCache.Used(_userPage.page.headLen);
	_userPage.page.bodyLen = 0;
	uint32_t bodyLenOld = 0;
	for (uint8_t n = 0; n < _userPage.page.head.segmentNumber; n++) {
		uint8_t seg = _userPage.page.head.segment[n];
		_userPage.page.bodyLen += seg;
		if (seg != 0xff) {
			_userPage.packetLen[_userPage.packetNum] = _userPage.page.bodyLen - bodyLenOld;
			_userPage.packetNum++;
			bodyLenOld = _userPage.page.bodyLen;
		}
	}
	_userPage.bodyLenRem = _userPage.page.bodyLen;
	LOG_OGG("%d,%d", _userPage.page.headLen, _userPage.page.bodyLen);
	_userPage.page.body = (uint8_t*)_memory.malloc_cb(_userPage.page.bodyLen);
	if (!_userPage.page.body) return OGG_RET_FAIL;
	_stage = Stage_e::STAGE_USER_COMMENT_BODY;
	return OGG_RET_SUCCESS;
}
OggRet_t OggDemuxer_c::DemuxUserCommentBody(OggData_c& oData) {
	uint32_t copyByte = _iCache.Size();
	copyByte = copyByte > _userPage.bodyLenRem ? _userPage.bodyLenRem : copyByte;
	gasf_memcpy(_userPage.page.body + (_userPage.page.bodyLen - _userPage.bodyLenRem), _iCache.Data(), copyByte);
	_iCache.Used(copyByte);
	_userPage.bodyLenRem -= copyByte;
	if (_userPage.bodyLenRem) return OGG_RET_MORE_DATA;
	uint32_t packetLenAcc = 0;
	for (uint8_t pn = 0; pn < _userPage.packetNum; pn++) {
		_userPage.packet[pn] = _userPage.page.body + packetLenAcc;
		packetLenAcc += _userPage.packetLen[pn];
	}
	//for (uint8_t pn = 0; pn < _userPage.packetNum; pn++) {
	for (uint8_t pn = 0; pn < 1; pn++) {
		uint8_t* iBuf = _userPage.packet[pn];
		uint32_t iSize = _userPage.packetLen[pn];
		uint8_t tagLen = 0;
		if (SyncString(iBuf, 7, (uint8_t*)"\x03vorbis")) {
			tagLen = 7;
		}
		else if (SyncString(iBuf, 7, (uint8_t*)"\x05vorbis")) {
			continue;
		}
		else if (SyncString(iBuf, 8, (uint8_t*)"OpusTags")) {
			tagLen = 8;
		}
		else {
			continue;
			//return OGG_RET_FAIL;
		}
		iBuf += tagLen;
		_userPage._vendorStringLen = (uint32_t)iBuf[0]
			| (((uint32_t)iBuf[1]) << 8)
			| (((uint32_t)iBuf[2]) << 16)
			| (((uint32_t)iBuf[3]) << 24);
		iBuf += 4;
		_userPage._vendorString = iBuf;
		iBuf += _userPage._vendorStringLen;
		_userPage._commentListLength = (uint32_t)iBuf[0]
			| (((uint32_t)iBuf[1]) << 8)
			| (((uint32_t)iBuf[2]) << 16)
			| (((uint32_t)iBuf[3]) << 24);
		iBuf += 4;
		uint32_t commentListNow = 0;
		for (; commentListNow < _userPage._commentListLength; commentListNow++) {
			uint32_t userCommentStringLen = (uint32_t)iBuf[0]
				| (((uint32_t)iBuf[1]) << 8)
				| (((uint32_t)iBuf[2]) << 16)
				| (((uint32_t)iBuf[3]) << 24);
			if (userCommentStringLen < 6) continue;
			iBuf += 4;
			uint8_t block_header[7];
			gasf_memcpy(block_header, iBuf, 7);
			Str2Low(block_header, 7);
			if (!_userPage._title && SyncString(block_header, 6, (uint8_t*)"title=")) {
				_userPage._titleLen = userCommentStringLen - 6;
				_userPage._title = iBuf;
			}
			else if (!_userPage._artist && SyncString(block_header, 7, (uint8_t*)"artist=")) {
				_userPage._artistLen = userCommentStringLen - 7;
				_userPage._artist = iBuf;
			}
			else if (!_userPage._album && SyncString(block_header, 6, (uint8_t*)"album=")) {
				_userPage._albumLen = userCommentStringLen - 6;
				_userPage._album = iBuf;
			}
			iBuf += userCommentStringLen;
		}
	}
	_stage = Stage_e::STAGE_DATA_FIRST_HEAD;
	return OGG_RET_SUCCESS;
}

OggRet_t OggDemuxer_c::DemuxDataHead(OggData_c& oData) {
	int32_t len = _iCache.Size();
	if (len < _syncStrLen) return OGG_RET_MORE_DATA;
	uint32_t syncOffset = 0;
	bool ret = SyncString((uint8_t*)_iCache.Data(), len, (uint8_t*)_syncStr, _syncStrLen, 0, &syncOffset);
	_iCache.Used(syncOffset);
	if (!ret) return OGG_RET_MORE_DATA;
	if (_iCache.Size() < _headByteCom) return OGG_RET_MORE_DATA;
	gasf_memcpy(&_dataPage.page.head, _iCache.Data(), _headByteCom);
	_dataPage.page.headLen = _headByteCom + _dataPage.page.head.segmentNumber;
	if (_iCache.Size() < (_dataPage.page.headLen)) return OGG_RET_MORE_DATA;
	gasf_memcpy(&_dataPage.page.head, _iCache.Data(), _dataPage.page.headLen);
	_iCache.Used(_dataPage.page.headLen);
	_dataPage.page.bodyLen = 0;
	_dataPage.packNum = 0;
	for (uint8_t n = 0; n < _dataPage.page.head.segmentNumber; n++) {
		uint8_t seg = _dataPage.page.head.segment[n];
		_dataPage.page.bodyLen += seg;
		if (seg != 0xff)
			_dataPage.packNum += 1;
	}
	_dataPage.bodyLenRem = _dataPage.page.bodyLen;
	_dataPage.packByte = 0;
	_dataPage.packIndex = 0;
	_dataPage.segmentIndex = -1;
	//LOG_OGG("%d", _dataPage.page.bodyLen);
	_stage = Stage_e::STAGE_DATA_BODY;
	return OGG_RET_SUCCESS;
}

OggRet_t OggDemuxer_c::DemuxDataBody(OggData_c& oData) {
	if (!_dataPage.packByte) {
		uint32_t packByte = 0;
		uint8_t index = _dataPage.segmentIndex + 1;
		for (; index < _dataPage.page.head.segmentNumber; index++) {
			uint8_t seg = _dataPage.page.head.segment[index];
			packByte += seg;
			if (seg != 0xff)
				break;
		}
		_dataPage.packByte = packByte;
		_dataPage.segmentIndex = index;
		_dataPage.packIndex++;
	}
	if (oData.LeftSize() < _dataPage.packByte)return OGG_RET_OUT_BUFF_NOT_ENOUGH;
	if (_iCache.Size() < _dataPage.packByte)return OGG_RET_MORE_DATA;
	oData.Append(_iCache.Data(), _dataPage.packByte);
	_iCache.Used(_dataPage.packByte);
	_dataPage.packByte = 0;
	if (_dataPage.segmentIndex >= (_dataPage.page.head.segmentNumber - 1))
		_stage = Stage_e::STAGE_DATA_HEAD;
	if (_dataPage.page.head.segment[_dataPage.page.head.segmentNumber - 1] == 0xff)
		return OGG_RET_INCOMPLETE;
	return OGG_RET_SUCCESS;
}

OggRet_t OggDemuxer_c::DemuxEos(OggData_c& oData) {
	return OGG_RET_SUCCESS;
}





