#include "MTF.Element.h"
#include "MTF.String.h"

MTF_Element::MTF_Element()
{
}
MTF_Element::~MTF_Element()
{
}

mtf_i32 MTF_Element::Set(const char* key, mtf_void* val)
{
	return MTF_Object::Set(key,val);
}
mtf_i32 MTF_Element::Get(const char* key, mtf_void* val)
{
	return MTF_Object::Get(key, val);
}
