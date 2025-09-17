#include "MTF.OpusMuxer.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MTF.Porting.h"

static const char* type_this = "opus_muxer";

void mtf_opus_muxer_register()
{
	MTF_Objects::Registe<MTF_OpusMuxer>(type_this);
}
MTF_OpusMuxer::MTF_OpusMuxer()
{

}

MTF_OpusMuxer::~MTF_OpusMuxer()
{
	if (_pFile)
		FileClosePorting(_pFile);
}

mtf_i32 MTF_OpusMuxer::Init()
{
	MTF_PRINT();
	if (!_url) {
		MTF_PRINT("error, _url = 0");
		return -1;
	}
	_pFile = FileOpenPorting(_url, "wb+");
	if (!_pFile) {
		MTF_PRINT("error, no such file:%s", _url);
		return -1;
	}
	return 0;
}

mtf_i32 MTF_OpusMuxer::receive(MTF_Data& iData)
{
	if (iData._flags & MTF_DataFlag_EMPTY) {
		return 0;
	}
	mtf_u8 head[8];
	MTF_MEM_SET(head, 0, sizeof(head));
	head[0] = (iData._size << 24) & 0xff;
	head[1] = (iData._size << 16) & 0xff;
	head[2] = (iData._size << 8) & 0xff;
	head[3] = (iData._size) & 0xff;
	FileWritePorting(_pFile, head, 8);
	FileWritePorting(_pFile, iData.Data(), iData._size);
	iData.Used(iData._size);
	return 0;
}

mtf_i32 MTF_OpusMuxer::Set(const char* key, mtf_void* val)
{
#if 1
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const char*)val);
		_url = (const char*)val;
		return 0;
	}
#endif
	return MTF_AudioMuxer::Set(key, val);
}
mtf_i32 MTF_OpusMuxer::Get(const char* key, mtf_void* val)
{
	return MTF_AudioMuxer::Get(key, val);
}
