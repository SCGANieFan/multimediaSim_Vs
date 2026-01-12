#include "MTF.Process.h"
#include "MTF.String.h"
using namespace mtf_ns;

MTF_Process::MTF_Process()
{
	_from = 0;
	_to = 0;
}
MTF_Process::~MTF_Process()
{

}


mtf_i32 MTF_Process::Pull(MTF_Data*& iData)
{
	if (!_from)
	{
		MTF_PRINT("err");
		return -1;
	}
	return _from->Generate(iData);
}
mtf_i32 MTF_Process::Push(MTF_Data& oData)
{
	if (!_to)
	{
		MTF_PRINT("err");
		return -1;
	}
	return _to->Receive(oData);
}
mtf_i32 MTF_Process::Receive(MTF_Data& iData)
{
	if (receive(iData) < 0)
		return -1;
	
	MTF_Data* oData = 0;
	if (generate(oData) < 0)
		return -1;
	if (Push(*oData) < 0)
		return -1;
	return 0;
}
mtf_i32 MTF_Process::Generate(MTF_Data*& oData)
{
	MTF_Data* iData;
	if (Pull(iData) < 0)
		return -1;
	if (receive(*iData) < 0)
		return -1;
	if (generate(oData))
		return -1;
	return 0;
}
mtf_i32 MTF_Process::Run()
{
	MTF_Data* iData;
	if (Pull(iData) < 0)
		return -1;

	if (receive(*iData) < 0)
		return -1;

	MTF_Data* oData;
	if (generate(oData) < 0)
		return -1;
	if (Push(*oData) < 0)
		return -1;
	return 0;
}

mtf_i32 MTF_Process::Set(const char* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "from")) {
		_from = (MTF_Element*)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "to")) {
		_to = (MTF_Element*)val; return 0;
	}
	return MTF_Element::Set(key, val);
}

mtf_i32 MTF_Process::Get(const char* key, mtf_void* val)
{
	return MTF_Element::Get(key, val);
}
