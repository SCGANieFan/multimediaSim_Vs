#pragma once
#include"MAF.Audio.h"

class MAFA_MusicPlc32 :public MAF_Audio
{
public:
	MAFA_MusicPlc32();
	~MAFA_MusicPlc32();
public:
	virtual maf_int32 Init() final;
	virtual maf_int32 Deinit() final;
	virtual maf_int32 Process(MAF_Data* dataIn, MAF_Data* dataOut) final;

	virtual maf_int32 Set(const maf_int8* key, maf_void* val) final;
	virtual maf_int32 Get(const maf_int8* key, maf_void* val) final;
private:
	maf_void* _hd = 0;
	maf_int32 _hdSize = 0;

	maf_int16 _decayMs = 0;
	maf_int16 _overlapMs = 0;
};
