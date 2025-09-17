#if 0
#include "MAF.demo.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include"audiodemo.h"

maf_void maf_audio_demo_register()
{
	MAF_Object::Registe<MAF_AudioDemo>("audio_demo");
}

MAF_AudioDemo::MAF_AudioDemo()
{
}
MAF_AudioDemo::~MAF_AudioDemo()
{
}

maf_int32 MAF_AudioDemo::Init()
{
	MAF_PRINT();
	AudioDemoInit();
	return 0;
}

maf_int32 MAF_AudioDemo::Deinit()
{
	MAF_PRINT();
	return 0;
}

maf_int32 MAF_AudioDemo::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	dataOut->Append(dataIn->GetData(), dataIn->GetSize());
	dataIn->Used(dataIn->GetSize());
	AudioDemoRun();
	return 0;
}

maf_int32 MAF_AudioDemo::Set(const maf_int8* key, maf_void* val)
{
#if 0
	if (MAF_String::StrCompare(key, "decayMs")){
		_decayMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (maf_int16)val; return 0;
	}
#endif
	return MAF_Audio::Set(key, val);
}

maf_int32 MAF_AudioDemo::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}


#endif