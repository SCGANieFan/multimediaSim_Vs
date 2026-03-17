#pragma once
#include"MTF.AudioDemuxer.h"
class MTF_OggDemuxer :public MTF_AudioDemuxer
{
public:
	MTF_OggDemuxer();
	~MTF_OggDemuxer();

protected:
	virtual mtf_i32 Init() final;
	//virtual mtf_i32 DeInit() final;
	virtual mtf_i32 generate(MTF_Data*& oData) final;
public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) final;
	virtual mtf_i32 Get(const char* key, mtf_void* val) final;

private:
	void* _pFile = 0;
	const char* _url = 0;
private:
	MTF_Data _oData;
	MTF_Data _tmpData;
private:
	void* _idDemuxer = 0;
	mtf_void* _hd = 0;
	mtf_i32 _hdSize = 0;
	mtf_u32 _hsize = 0;
	mtf_u32 _totalsize = 0;
	mtf_u32 _firstPageSize = 0;
	mtf_u32 _firstPacketNum = 0;
	mtf_u32 _lastPageSize = 0;
	mtf_u32 _lastPacketNum = 0;
	mtf_u64 _lastGranulePos = 0;
	mtf_u64 _granulePosNow = 0;
	mtf_u32 _pageNum = 0;
	mtf_u32 _fcnt = 0;
	mtf_u32 _fsize = 0;
	mtf_u32 _duration = 0;
	uint8_t* _idPageBody = 0;
	uint32_t _idPageBodyLen = 0;
	uint8_t* _title = 0;
	uint32_t _titleLen = 0;
	uint8_t* _artist = 0;
	uint32_t _artistLen = 0;
	uint8_t* _album = 0;
	uint32_t _albumLen = 0;
	uint32_t **_userPacket = 0;
	uint32_t *_userPacketLen = 0;
	uint32_t _userPacketNum = 0;
}; 

