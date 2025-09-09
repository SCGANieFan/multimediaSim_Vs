#include "MTF.Source.h"
#include "MTF.String.h"
using namespace mtf_ns;

MTF_Source::MTF_Source()
{
	_to = 0;
}
MTF_Source::~MTF_Source()
{
}

mtf_i32 MTF_Source::Push(MTF_Data& oData)
{
	if (!_to)
	{
		MTF_PRINT("err");
		return -1;
	}
	_to->Receive(oData);
}

mtf_i32 MTF_Source::Run()
{
	MTF_Data* oData;
	if (generate(oData) < 0)
		return -1;
	if (Push(*oData) < 0)
		return -1;
	return 0;
}

mtf_i32 MTF_Source::Generate(MTF_Data*& oData)
{
	generate(oData);
	return 0;
}

mtf_i32 MTF_Source::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "to"))
	{
		_to = (MTF_Element*)val; return 0;
	}
	return MTF_Element::Set(key, val);
}

mtf_i32 MTF_Source::Get(const char* key, mtf_void* val)
{
	return MTF_Element::Get(key, val);
}






