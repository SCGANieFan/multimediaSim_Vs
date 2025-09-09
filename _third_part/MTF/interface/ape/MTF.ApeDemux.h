#pragma once
#include"MTF.AudioDemuxer.h"
class MTF_ApeDemux :public MTF_AudioDemuxer
{

private:
	typedef struct {
		mtf_void* apeHeader;
		mtf_u32 startFrame;
		mtf_u32 skip;
	}ExtraInfo_t;

	class SeekTableManger{
	public:
		SeekTableManger() {}
		~SeekTableManger() {
			if(_seektableRead)
				MTF_FREE(_seektableRead);
		}
	public:
		void Init(int seekTablePos ,int seekTableSizeByte, void *pFile) {
			_seekTablePos = seekTablePos;
			_seekTableSizeByte = seekTableSizeByte;
			_seekTableUsedSizeByte = 0;
			_seekTableNum = _seekTableSizeByte >> 2;
			const uint32_t tableNumTmp = 2;
			_seektableReadSizeByte = tableNumTmp * 4;
			_seektableRead = (mtf_i32*)MTF_MALLOC(_seektableReadSizeByte);
			_pFile = pFile;
			fseek((FILE*)_pFile, _seekTablePos, SEEK_SET);

		}
		void UpdataSeektable() {
			_seektableReadValidSizeByte = _seekTableSizeByte - _seekTableUsedSizeByte;
			_seektableReadValidSizeByte = _seektableReadValidSizeByte < _seektableReadSizeByte ? _seektableReadValidSizeByte : _seektableReadSizeByte;
			if(_seektableReadValidSizeByte)
				fread(_seektableRead, 1, _seektableReadValidSizeByte, (FILE*)_pFile);
		}
		mtf_i32 GetValidSeekTableNum() {
			return _seektableReadValidSizeByte>>2;
		}
		mtf_i32* GetValidSeekTable() {
			return _seektableRead;
		}
		mtf_i32 GetValidSeekTableByte() {
			return _seektableReadValidSizeByte;
		}
		void Used() {
			_seekTableUsedSizeByte += _seektableReadValidSizeByte;
			_seektableReadValidSizeByte = 0;
		}
	public:
		mtf_i32 _seekTablePos = 0;
		mtf_i32 _seekTableSizeByte = 0;
		mtf_i32 _seekTableUsedSizeByte = 0;
		mtf_i32 _seekTableNum = 0;

		mtf_i32* _seektableRead = 0;
		mtf_i32 _seektableReadValidSizeByte = 0;
		mtf_i32 _seektableReadSizeByte = 0;
		void* _pFile = 0;
	};


public:
	MTF_ApeDemux();
	~MTF_ApeDemux();
protected:
	virtual mtf_i32 Init() final;
	//virtual mtf_i32 receive(MTF_Data& iData) final;
	virtual mtf_i32 generate(MTF_Data*& oData) final;
public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) final;
	virtual mtf_i32 Get(const char* key, mtf_void* val) final;



private:
	MTF_Data _oData;
	mtf_void* _hd = 0;
	mtf_i32 _hdSize = 0;
	mtf_i32 _frames = 0;
	mtf_u32 _startPos = 0;
	ExtraInfo_t _extraInfo;
	mtf_bool _isFirstFrame = false;
private:
	void* _pFile = 0;
	const char* _url = 0;
};

