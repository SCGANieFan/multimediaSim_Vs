#include "MTF.demo.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MAF.h"

void mtf_audio_demo_register()
{
	MTF_Objects::Registe<MTF_AudioDemo>("audio_demo");
}
MTF_AudioDemo::MTF_AudioDemo()
{

}

MTF_AudioDemo::~MTF_AudioDemo()
{
	if (_iData.Data())
	{
		_iData.Used(_iData._size);
		MTF_FREE(_iData.Data());
	}
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
	if (_hd)
	{
		MTF_FREE(_hd);
	}
}

mtf_i32 MTF_AudioDemo::Init()
{	
	//lib init

	//io data
	mtf_i32 size = _frameBytes;
	_iData.Init((mtf_u8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_u8*)MTF_MALLOC(size), size);

	return 0;
}

mtf_i32 MTF_AudioDemo::receive(MTF_Data& iData)
{
	_iData.Append(iData.Data(), iData._size);
	iData.Used(iData._size);
	return 0;
}


mtf_i32 MTF_AudioDemo::generate(MTF_Data*& oData)
{

	_frames++;
	return 0;
}

mtf_i32 MTF_AudioDemo::Set(const char* key, mtf_void* val)
{
#if 0
	if (MTF_String::StrCompare(key, "decayMs")) {
		_decayMs = (mtf_i16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (mtf_i16)val; return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_i32 MTF_AudioDemo::Get(const char* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
