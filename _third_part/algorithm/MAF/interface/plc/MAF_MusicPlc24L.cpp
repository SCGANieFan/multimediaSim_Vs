#if 0
#include<string.h>

#include "MAF_MusicPlc24L.h"
#include "MAF.Objects.h"
#include "MAF.Interface.MusicPlc.h"


#include"musicPlc24bL.h"

maf_void maf_algorithm_audio_music_plc24L_register()
{
	MAF_Object::Registe<MAF_MusicPlc24L>("proc-mplc24L");
}

MAF_MusicPlc24L::MAF_MusicPlc24L()
{
}
MAF_MusicPlc24L::~MAF_MusicPlc24L()
{
}


maf_int32 MAF_MusicPlc24L::Init()
{
	MAF_PRINT();
	AA_MusicPlcParam* musicPlcParam = (AA_MusicPlcParam*)param;

	MusicPlcSampleParam sampleParam;
	memset(&sampleParam, 0, sizeof(MusicPlcSampleParam));
	sampleParam.fsHz = musicPlcParam->fsHz;
	sampleParam.channels = musicPlcParam->channels;
	sampleParam.frameSamples = musicPlcParam->frameSamples;

	_fsHz = musicPlcParam->fsHz;
	_channels = musicPlcParam->channels;
	_frameSamples = musicPlcParam->frameSamples;


	_hdSize = MusicPlc24bLGetStateSize(
		musicPlcParam->overlapMs,
		&sampleParam);

	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (unsigned)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	maf_int32 ret = MusicPlc24bLStateInit(
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

maf_int32 MAF_MusicPlc24L::Deinit()
{
	MAF_PRINT();
	_memory.Free(_hd);
	return 0;
}

maf_int32 MAF_MusicPlc24L::Process(MAF_AudioFrame* frameIn, MAF_AudioFrame* frameOut)
{
	maf_int32 inUsed = 0;
	maf_int32 ret;

	maf_int32 outByte = frameOut->max - frameOut->off - frameOut->size;

	if (frameIn->flags & MAFA_FRAME_IS_EMPTY)
	{
		ret = MusicPlc24bL(
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
		ret = MusicPlc24bL(
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


#if 1	
	maf_uint32* ptr = (maf_uint32*)frameOut->buff + frameOut->off + frameOut->size;
	for (maf_int32 i = 0; i < outByte/sizeof(maf_uint32); i++)
	{
		ptr[i] = ptr[i] << 8;
	}
#endif


	frameIn->size -= inUsed;
	frameIn->off += inUsed;
	frameOut->size += outByte;
	return 0;
}


#endif