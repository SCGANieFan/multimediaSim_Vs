#include"MTF.OggDemuxer.h"
#include"MTF.Objects.h"
#include"MTF.Porting.h"
#include"ogg_api.h"


using namespace mtf_ns;
void mtf_ogg_demuxer_register()
{
	MTF_Objects::Registe<MTF_OggDemuxer>("ogg_demuxer");
}	


MTF_OggDemuxer ::MTF_OggDemuxer ()
{

}

MTF_OggDemuxer ::~MTF_OggDemuxer ()
{
	if (_pFile)
		FileClosePorting(_pFile);
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
	if (_tmpData.Data())
	{
		_tmpData.Used(_tmpData._size);
		MTF_FREE(_tmpData.Data());
	}

	if (_hd)
	{
		MTF_FREE(_hd);
	}
	if (_idDemuxer) {
		ogg_api_demuxer_close(_idDemuxer);
		ogg_api_demuxer_destory(_idDemuxer);
		_idDemuxer = 0;
	}
}

#if 1

static void* OggMalloc(uint32_t size) {
	static int32_t sizeTotal = 0;
#if 1
	sizeTotal += size;
	void* ptr = malloc(size);
	MTF_PRINT("malloc, ptr:%p, size:%d, sizeTotal:%d,", ptr, size, sizeTotal);
	return ptr;
#else
	return ((ALGO_Malloc_t)_malloc)(size);
#endif
}

static void* OggRealloc(void* ptr, uint32_t size) {
	void* ptrNew = realloc(ptr, size);
	MTF_PRINT("realloc, (%p->%p,%d)", ptr, ptrNew, size);
	return ptrNew;
}

static void OggFree(void* ptr) {
#if 1
	MTF_PRINT("free, ptr:%p", ptr);
#endif
	return free(ptr);
}
#include <stdio.h>
static void OggPrint(const char* buf, uint32_t len) {
#if 0
	static char buf[256];
	VaListPorting_t args;
	VaStartPorting(args, fmt);
	VsprintfPorting(buf, fmt, args);
	VaEndPorting(args);
#if 0
	MTF_PRINTORI("%s", buf);
#else
	printf("%s", buf);
#endif
#else
	printf("%s", buf);
#endif
}
#endif

mtf_i32 MTF_OggDemuxer::Init()
{
	MTF_PRINT();
	if (!_url) {
		MTF_PRINT("error, _url = 0");
		return -1;
	}
	_pFile = FileOpenPorting(_url, "rb+");
	if (!_pFile) {
		MTF_PRINT("error, no such file:%s", _url);
		return -1;
	}

	OggApiBasePort_t bp;
	bp.malloc_cb = OggMalloc;
	bp.realloc_cb = OggRealloc;
	bp.free_cb = OggFree;
	bp.printf_cb = OggPrint;
	_idDemuxer = ogg_api_demuxer_create(&bp);
	if (!_idDemuxer) return -1;
	OggApiRet_t ret = OGG_API_RET_SUCCESS;
	ret |= ogg_api_demuxer_open(_idDemuxer);
	if (ret != OGG_API_RET_SUCCESS) return -1;

	mtf_i32 size = 4*1024;
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);
	_tmpData.Init((mtf_u8*)MTF_MALLOC(1024), 1024);
	while (1) {
		ret = ogg_api_demuxer_get(_idDemuxer, "stageHead", 0);
		if (ret == OGG_API_RET_SUCCESS) break;
		_tmpData.Clear();
		mtf_i32 readedSize = FileReadPorting(_pFile, _tmpData.LeftData(), _tmpData.LeftSize());
		if (!readedSize) {
			return -1;
		}
		_tmpData._size += readedSize;
		int32_t sizeio = _tmpData._size;
		ret = ogg_api_demuxer_receive(_idDemuxer, _tmpData.Data(), &sizeio);
		_tmpData.Used(sizeio);
		ret = ogg_api_demuxer_generate(_idDemuxer, 0, 0);
	}

	uint32_t getData = 0;
	ret = OGG_API_RET_SUCCESS;
	ret |= ogg_api_demuxer_get(_idDemuxer, "rate", &getData); _rate = getData;
	ret |= ogg_api_demuxer_get(_idDemuxer, "ch", &getData); _ch = getData;
	_hsize = 0;
	ret |= ogg_api_demuxer_get(_idDemuxer, "idPageLen", &getData); _hsize += getData;
	ret |= ogg_api_demuxer_get(_idDemuxer, "userPageLen", &getData); _hsize += getData;
	ret |= ogg_api_demuxer_get(_idDemuxer, "idPageBody", &_idPageBody);
	ret |= ogg_api_demuxer_get(_idDemuxer, "idPageBodyLen", &_idPageBodyLen);
	ogg_api_demuxer_get(_idDemuxer, "title", &_title);
	ogg_api_demuxer_get(_idDemuxer, "titleLen", &_titleLen);
	ogg_api_demuxer_get(_idDemuxer, "artist", &_artist);
	ogg_api_demuxer_get(_idDemuxer, "artistLen", &_artistLen);
	ogg_api_demuxer_get(_idDemuxer, "album", &_album);
	ogg_api_demuxer_get(_idDemuxer, "albumLen", &_albumLen);
	ogg_api_demuxer_get(_idDemuxer, "userPagePacketNum", &_userPacketNum);
	ogg_api_demuxer_get(_idDemuxer, "userPagePacket", &_userPacket);
	ogg_api_demuxer_get(_idDemuxer, "userPagePacketLen", &_userPacketLen);
	if (ret) return -1;
	for (uint32_t n = 0; n < _userPacketNum; n++) {
		MTF_PRINT("packet[%d]:%p,%d", n, _userPacket[n], _userPacketLen[n]);
	}
	FileSeekPorting(_pFile, 0, FileSeekPorting_e::FILE_PORTING_SEEK_END);
	_totalsize = FileTellPorting(_pFile);
	FileSeekPorting(_pFile, 0, FileSeekPorting_e::FILE_PORTING_SEEK_SET);
	_totalsize -= _hsize;
	MTF_PRINT("%d hz, %d ch, %u, %u", _rate, _ch, _hsize, _totalsize);
	MTF_PRINT("%u,(0x%x,0x%x,0x%x,0x%x)", _idPageBodyLen, 
		((uint32_t*)_idPageBody)[0], 
		((uint32_t*)_idPageBody)[1], 
		((uint32_t*)_idPageBody)[2], 
		((uint32_t*)_idPageBody)[3]);

	//first data page head
	ogg_api_demuxer_set(_idDemuxer, "resetToData", 0);
	_tmpData.Used(_tmpData._size);
	FileSeekPorting(_pFile, _hsize, FileSeekPorting_e::FILE_PORTING_SEEK_SET);
	while (1) {
		_tmpData.Clear();
		mtf_i32 readedSize = FileReadPorting(_pFile, _tmpData.LeftData(), _tmpData.LeftSize());
		if (!readedSize) return -1;
		_tmpData._size += readedSize;
		int32_t sizeio = _tmpData._size;
		ret = ogg_api_demuxer_receive(_idDemuxer, _tmpData.Data(), &sizeio);
		_tmpData.Used(sizeio);
		ret = ogg_api_demuxer_generate(_idDemuxer, 0, 0);
		ret = ogg_api_demuxer_get(_idDemuxer, "dataPageLen", &getData);
		if (ret == OGG_API_RET_SUCCESS) {
			_firstPageSize = getData;
			ogg_api_demuxer_get(_idDemuxer, "dataPacketNum", &getData);
			_firstPacketNum = getData;
			break;
		}
	}

	//last data page off
	mtf_i32 offLastPage = 0;
	offLastPage += _tmpData.Max();
	while (1) {
		_tmpData.Used(_tmpData._size);
		_tmpData.Clear();
		FileSeekPorting(_pFile, -offLastPage, FileSeekPorting_e::FILE_PORTING_SEEK_END);
		mtf_i32 readedSize = FileReadPorting(_pFile, _tmpData.LeftData(), _tmpData.LeftSize());
		if (!readedSize) return -1;
		_tmpData._size += readedSize;
		if (_tmpData._size < 4) {
			return -1;
		}
		mtf_i32 size = _tmpData._size - 4;
		uint8_t* ptr = (uint8_t*)_tmpData.Data() + size;
		for (; size >= 0; size--) {
			if (ptr[0] == 'O') {
				if (ptr[1] == 'g'
					&& ptr[2] == 'g'
					&& ptr[3] == 'S') {
					offLastPage -= size;
					break;
				}
			}
			ptr--;
		}
		if (size >= 0) 
			break;
		offLastPage += _tmpData._size - 4;
	}

	//last data page head
	ogg_api_demuxer_set(_idDemuxer, "resetToData", 0);
	_tmpData.Used(_tmpData._size);
	FileSeekPorting(_pFile, -offLastPage, FileSeekPorting_e::FILE_PORTING_SEEK_END);
	while (1) {
		_tmpData.Clear();
		mtf_i32 readedSize = FileReadPorting(_pFile, _tmpData.LeftData(), _tmpData.LeftSize());
		if (!readedSize) return -1;
		_tmpData._size += readedSize;
		int32_t sizeio = _tmpData._size;
		ret = ogg_api_demuxer_receive(_idDemuxer, _tmpData.Data(), &sizeio);
		_tmpData.Used(sizeio);
		ret = ogg_api_demuxer_generate(_idDemuxer, 0, 0);
		ret = ogg_api_demuxer_get(_idDemuxer, "dataPageLen", &getData);
		if (ret == OGG_API_RET_SUCCESS) {
			_lastPageSize = getData;
			ogg_api_demuxer_get(_idDemuxer, "dataPageNum", &getData);
			_pageNum = getData > 2 ? getData - 2 : 0;
			ogg_api_demuxer_get(_idDemuxer, "dataPacketNum", &getData);
			_lastPacketNum = getData;
			ogg_api_demuxer_get(_idDemuxer, "granulePos", &_lastGranulePos);
			break;
		}
	}
	MTF_PRINT("(%u,%u),(%u,%u),%u,%llu", _firstPageSize, _lastPageSize, _firstPacketNum, _lastPacketNum, _pageNum, _lastGranulePos);
	_fcnt = _pageNum * (_lastPacketNum + _firstPacketNum) / 2;
	_fsize = _totalsize / _fcnt;
	_duration = _lastGranulePos * 10 / (_rate / 100);
	MTF_PRINT("%u,%u,%u ms", _fcnt, _fsize, _duration);

	ogg_api_demuxer_set(_idDemuxer, "resetToData", 0);
	FileSeekPorting(_pFile, _hsize, FileSeekPorting_e::FILE_PORTING_SEEK_SET);
	_tmpData.Used(_tmpData._size);
	return 0;
}


mtf_i32 MTF_OggDemuxer::generate(MTF_Data*& oData)
{
	_oData.Clear();
	while (1) {
		int32_t oByte = _oData.LeftSize();
		OggApiRet_t ret = ogg_api_demuxer_generate(_idDemuxer, _oData.LeftData(), &oByte);
		if (ret == OGG_API_RET_MORE_DATA) {
			_tmpData.Clear();
			mtf_i32 readedSize = FileReadPorting(_pFile, _tmpData.LeftData(), _tmpData.LeftSize());
			if (!readedSize) {
				_oData._flags |= MTF_DataFlag_ESO;
				break;
			}
			_tmpData._size += readedSize;
			int32_t size = _tmpData._size;
			OggApiRet_t ret = ogg_api_demuxer_receive(_idDemuxer, _tmpData.Data(), &size);
			_tmpData.Used(size);
			continue;
		}
		_oData._size += oByte;
		if (ret == OGG_API_RET_INCOMPLETE) {
			_oData._flags |= MTF_DataFlag_INCOMPLETE;
		}
#if 0
		ogg_api_demuxer_get(_idDemuxer, "granulePos", &_granulePosNow);
		MTF_PRINT("%llu ms/%llu ms", _granulePosNow/(_rate / 1000), _lastGranulePos/(_rate / 1000));
#endif
		break;
	}
	oData = &_oData;
	return 0;
}


mtf_i32 MTF_OggDemuxer ::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const char*)val);
		_url = (const char*)val;

		return 0;
	}
	return MTF_AudioDemuxer::Set(key, val);
}
mtf_i32 MTF_OggDemuxer ::Get(const char* key, mtf_void* val)
{
	return MTF_AudioDemuxer::Get(key, val);
}




