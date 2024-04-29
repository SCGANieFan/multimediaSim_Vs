#if 1
#include "MAF_MusicPlc16.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "MAF.Interface.MusicPlc.h"
#include "musicPlc16b.h"

maf_void maf_music_plc16_register()
{
	MAF_Object::Registe<MAF_MusicPlc16>("music_plc16");
}

MAF_MusicPlc16::MAF_MusicPlc16()
{
}
MAF_MusicPlc16::~MAF_MusicPlc16()
{
}

maf_int32 MAF_MusicPlc16::Init()
{
	MAF_PRINT();

	MusicPlcSampleParam sampleParam;
	MAF_MEM_SET(&sampleParam, 0, sizeof(MusicPlcSampleParam));
	sampleParam.fsHz = _rate;
	sampleParam.channels = _ch;
	sampleParam.frameSamples = _frameSamples;

	_hdSize = MusicPlc16bGetStateSize(
		_overlapMs,
		&sampleParam);
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (unsigned)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	maf_int32 ret = MusicPlc16bStateInit(
		_hd, 
		_overlapMs,
		_decayMs,
		&sampleParam);

	if (ret < 0)
	{
		return -1;
	}
	return 0;
}

maf_int32 MAF_MusicPlc16::Deinit()
{
	MAF_PRINT();
	_memory.Free(_hd);
	return 0;
}

maf_int32 MAF_MusicPlc16::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	maf_int32 inUsed = 0;
	maf_int32 ret;

	maf_int32 outByte = dataOut->GetLeftSize();
	
	if (dataIn->CheckFlag(MAFA_FRAME_IS_EMPTY))
	{
		dataIn->ClearFlag(MAFA_FRAME_IS_EMPTY);
		ret = MusicPlc16b(
			_hd, 
			NULL, 
			0,
			NULL,
			dataOut->GetLeftData(),
			&outByte, 
			true);
	}
	else
	{
		ret = MusicPlc16b(
			_hd,
			dataIn->GetData(),
			dataIn->GetSize(),
			&inUsed,
			dataOut->GetLeftData(),
			&outByte,
			false);
	}
	if (ret < 0)
	{
		return -1;
	}
	dataIn->Used(inUsed);
	dataIn->ClearUsed();
	dataOut->Append(outByte);
	return 0;
}

maf_int32 MAF_MusicPlc16::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "decayMs")){
		_decayMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (maf_int16)val; return 0;
	}
	return MAF_Audio::Set(key, val);
}

maf_int32 MAF_MusicPlc16::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}


#endif