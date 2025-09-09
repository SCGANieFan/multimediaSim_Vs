
#include "MTF.Object.h"
#include "MTF.String.h"


MTF_Object::MTF_Object()
{
}
MTF_Object::~MTF_Object()
{
}


mtf_i32 MTF_Object::Set(const char* key, mtf_void* val)
{
#if 0
	if (MTF_String::StrCompare(key, "tag"))
	{
		SetTag((const char*)val); return 0;
	}
#endif
	return -1;
}

mtf_i32 MTF_Object::Get(const char* key, mtf_void* val)
{
	return -1;
}


