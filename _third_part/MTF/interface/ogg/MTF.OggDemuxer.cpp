#include"MTF.OggDemuxer.h"
#include"MTF.Objects.h"
#include"MTF.Porting.h"
#include"ogg_api.h"


using namespace mtf_ns;
void mtf_ogg_demuxer_register()
{
	MTF_Objects::Registe<MTF_OggDemuxer>("ogg_demuxer");
	ogg_api_register_ogg_demuxer();
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
static void OggPrint(const char* fmt, ...) {
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
}
#endif


class Frame
{
public:
	Frame() {}
	~Frame() {}
public:
	int Left() { return max - offset - size; }
	void* LeftData() { return (char*)buff + offset + size; }
	void* Data() { return (char*)buff + offset; }
public:
	void* buff;
	int max;
	int offset;
	int size;
	unsigned index;
	unsigned timestamp;
	void* priv;
};

static uint8_t* SyncString(uint8_t* buf, uint32_t len, const char* str) {
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

static bool SyncStringFromFile(void *pFile, Frame &frm, const char* searchStr) {
	uint32_t searchStrLen = strlen(searchStr);
	while (1) {
		if (frm.size < searchStrLen) {
			if (frm.offset) {
				if (frm.size) {
					memmove(frm.buff, frm.Data(), frm.size);
				}
				frm.priv = (void*)((int)frm.priv + frm.offset);
				frm.offset = 0;
			}
			int readByte = frm.Left();
			int readByteActual = FileReadPorting(pFile, frm.LeftData(), readByte);
			frm.size += readByteActual;
			if (frm.size < searchStrLen) {
				return false;
			}
		}
		uint8_t* bufSync = SyncString((uint8_t*)frm.Data(), frm.size, searchStr);
		uint32_t bufOffset = frm.size - searchStrLen + 1;
		if (bufSync) bufOffset = bufSync - (uint8_t*)frm.Data();
		frm.offset += bufOffset;
		frm.size -= bufOffset;
		if (bufSync) break;
	}
	if (frm.offset) {
		if (frm.size) {
			memmove(frm.buff, frm.Data(), frm.size);
		}
		frm.priv = (void*)((int)frm.priv + frm.offset);
		frm.offset = 0;
	}
	int readByte = frm.Left();
	int readByteActual = FileReadPorting(pFile, frm.LeftData(), readByte);
	frm.size += readByteActual;
	return true;
}

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
	//ret |= ogg_api_demuxer_set(_idDemuxer, "", 0);
	//if (ret != OGG_API_RET_SUCCESS) return -1;

	ret |= ogg_api_demuxer_open(_idDemuxer);
	if (ret != OGG_API_RET_SUCCESS) return -1;

	mtf_i32 size = 4*1024;
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);
	_tmpData.Init((mtf_u8*)MTF_MALLOC(4096), 4096);
	while (1) {
		uint32_t getData = 0;
		if (!_rate) {
			ret = ogg_api_demuxer_get(_idDemuxer, "rate", &getData);
			if (ret == OGG_API_RET_SUCCESS) _rate = getData;
		}
		if (!_ch) {
			ret = ogg_api_demuxer_get(_idDemuxer, "ch", &getData);
			if (ret == OGG_API_RET_SUCCESS) _ch = getData;
		}
		if (_rate || _ch) {
			break;
		}
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
	MTF_PRINT("%d hz, %d ch", _rate, _ch);
	//ret |= ogg_api_demuxer_set(_idDemuxer, "", 0);
	//if (ret != OGG_API_RET_SUCCESS) return -1;

#if 1
	{
		Frame frm;
		memset(&frm, 0, sizeof(Frame));
		frm.max = 4 * 1024;
		frm.buff = malloc(frm.max);
		if (!frm.buff) return false;


		int offOri = FileTellPorting(_pFile);
		int pageNum = 0;
		int pageByte = 0;
		int packetNum = 0;
		uint64_t granulePos = 0;
		const char* syncStr = "OggS";
		uint32_t syncStrLen = strlen(syncStr);

		// first page
		bool ret = true;
		ret = SyncStringFromFile(_pFile, frm, syncStr);
		if (!ret) return -1;
		frm.offset += syncStrLen;
		frm.size -= syncStrLen;

		// second page
		ret = SyncStringFromFile(_pFile, frm, syncStr);
		if (!ret) return -1;
		frm.offset += syncStrLen;
		frm.size -= syncStrLen;
		uint32_t off0 = FileTellPorting(_pFile);
		FileSeekPorting(_pFile, 0, FileSeekPorting_e::FILE_PORTING_SEEK_END);
		uint32_t off1 = FileTellPorting(_pFile);
		uint32_t otherPageByte = off1 - off0;
		FileSeekPorting(_pFile, off0, FileSeekPorting_e::FILE_PORTING_SEEK_SET);


		// third page
		ret = SyncStringFromFile(_pFile, frm, syncStr);
		if (!ret) return -1;
		frm.priv = 0;
		if (frm.size < 27) return -1;
		uint8_t* head = (uint8_t*)frm.Data();
		uint32_t headByte = 27 + head[26];
		if (frm.size < headByte) return -1;
		for (int n = 0; n < head[26]; n++) {
			if (head[27 + n] < 255)
				packetNum++;
		}
		frm.offset += syncStrLen;
		frm.size -= syncStrLen;
		ret = SyncStringFromFile(_pFile, frm, syncStr);
		if (!ret) return -1;
		pageByte = (uint32_t)frm.priv;

		//last page
		int32_t seekEndPageOff = (int32_t)pageByte << 1;
		seekEndPageOff = seekEndPageOff > otherPageByte ? otherPageByte : seekEndPageOff;
		FileSeekPorting(_pFile, -seekEndPageOff, FileSeekPorting_e::FILE_PORTING_SEEK_END);

		frm.size = 0;
		frm.offset = 0;
		ret = SyncStringFromFile(_pFile, frm, syncStr);
		if (!ret) return -1;
		if (frm.size < 27)return -1;
		head = (uint8_t*)frm.Data();
		headByte = 27 + head[26];
		if (frm.size < headByte)return -1;
		for (int n = 0; n < head[26]; n++) {
			if (head[27 + n] < 255)
				packetNum++;
		}
		granulePos = 0;
		for (int n = 13; n > 5; n--) {
			granulePos = (granulePos << 8) | head[n];
		}
		pageNum = otherPageByte / pageByte;
		MTF_PRINT("[%d],pack:%d,%lld ms,file %d byte", pageNum, packetNum * pageNum / 2, granulePos / 44, otherPageByte);
		FileSeekPorting(_pFile, offOri, FileSeekPorting_e::FILE_PORTING_SEEK_SET);
		Free(frm.buff);
	}
#endif

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
			//static uint32_t cnt = 0;
			//MTF_PRINT("[%d] readedSize:%d", ++cnt, readedSize);
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




