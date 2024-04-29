#if 0
#include<string.h>

#include "MAF_MusicPlc32.h"
#include "MAF.Objects.h"
#include "MAF.Interface.MusicPlc.h"


#include"musicPlc32b.h"

maf_void maf_algorithm_audio_music_plc32_register()
{
	MAF_Object::Registe<MAF_MusicPlc32>("proc-mplc32");
}

MAF_MusicPlc32::MAF_MusicPlc32()
{
}
MAF_MusicPlc32::~MAF_MusicPlc32()
{
}


maf_int32 MAF_MusicPlc32::Init()
{
	MAF_PRINT();
	AA_MusicPlcParam* musicPlcParam = (AA_MusicPlcParam*)param;

	MusicPlcSampleParam sampleParam;
	memset(&sampleParam, 0, sizeof(MusicPlcSampleParam));
	sampleParam.fsHz = musicPlcParam->fsHz;
	sampleParam.channels = musicPlcParam->channels;
	sampleParam.frameSamples = musicPlcParam->frameSamples;

	_hdSize = MusicPlc32bGetStateSize(
		musicPlcParam->overlapMs,
		&sampleParam);

	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (unsigned)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	maf_int32 ret = MusicPlc32bStateInit(
		_hd,
		musicPlcParam->overlapMs,
		musicPlcParam->decayTimeMs,
		&sampleParam);

	if (ret < 0)
	{
		return -1;
	}

	return 0;
}

maf_int32 MAF_MusicPlc32::Deinit()
{
	MAF_PRINT();
	_memory.Free(_hd);
	return 0;
}

maf_int32 MAF_MusicPlc32::Process(MAF_AudioFrame* frameIn, MAF_AudioFrame* frameOut)
{
	maf_int32 inUsed = 0;
	maf_int32 ret;

	maf_int32 outByte = frameOut->max - frameOut->off - frameOut->size;

	if (frameIn->flags & MAFA_FRAME_IS_EMPTY)
	{
		ret = MusicPlc32b(
			_hd,
			NULL,
			0,
			NULL,
			frameOut->buff + frameOut->off + frameOut->size,
			&outByte,
			true);
	}
	else
	{
		ret = MusicPlc32b(
			_hd,
			frameIn->buff + frameIn->off,
			frameIn->size,
			&inUsed,
			frameOut->buff + frameOut->off + frameOut->size,
			&outByte,
			false);
	}
	if (ret < 0)
	{
		return -1;
	}
	frameIn->size -= inUsed;
	frameIn->off += inUsed;
	frameOut->size += outByte;
	return 0;
}


#endif