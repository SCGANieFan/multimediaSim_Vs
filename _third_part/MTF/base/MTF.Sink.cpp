#include "MTF.Sink.h"
#include "MTF.String.h"
using namespace mtf_ns;

MTF_Sink::MTF_Sink()
{
	_from = 0;
}
MTF_Sink::~MTF_Sink()
{
}


mtf_i32 MTF_Sink::Pull(MTF_Data*& iData)
{
	if (!_from)
	{
		MTF_PRINT("err");
		return -1;
	}
	return _from->Generate(iData);
}

mtf_i32 MTF_Sink::Receive(MTF_Data& iData)
{
	return  receive(iData);
}

mtf_i32 MTF_Sink::Run()
{
	MTF_Data* iData;
	if (Pull(iData) < 0)
		return -1;
	if (receive(*iData))
		return -1;
	return 0;
}

mtf_i32 MTF_Sink::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "from"))
	{
		_from = (MTF_Element*)val; return 0;
	}
	return MTF_Element::Set(key, val);
}
mtf_i32 MTF_Sink::Get(const char* key, mtf_void* val)
{
	return MTF_Element::Get(key, val);
}



