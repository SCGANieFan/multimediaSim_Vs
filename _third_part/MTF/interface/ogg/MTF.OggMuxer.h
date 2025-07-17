#pragma once
#include"MTF.AudioMuxer.h"
class MTF_OggMuxer :public MTF_AudioMuxer
{
public:
	MTF_OggMuxer();
	~MTF_OggMuxer();
protected:
	virtual mtf_int32 Init() final;
	virtual mtf_int32 receive(MTF_Data& iData) final;
public:
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;
private:
	static void* OggMalloc(uint32_t size);
	static void* OggRealloc(void* ptr, uint32_t size);
	static void OggFree(void* ptr);
	static void OggPrint(const char* fmt, ...);
private:
	void* _pFile = 0;
	const mtf_int8* _url = 0;
private:
	MTF_Data _oData;
	MTF_Data _head;
private:
	mtf_void* _hd = 0;
	mtf_int32 _hdSize = 0;
	mtf_int32 _page_byte_round = 4096;
};

