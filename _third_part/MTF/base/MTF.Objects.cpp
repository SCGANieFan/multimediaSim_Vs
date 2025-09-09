#include "MTF.Objects.h"
#include "MTF.String.h"

typedef mtf_void* (*Create_t)();

MTF_Objects MTF_Objects::objects;

MTF_Objects::MTF_Objects()
{
	MTF_MEM_SET((mtf_i8*)_items, 0, sizeof(_items));
}
MTF_Objects::~MTF_Objects()
{
}


void* MTF_Objects::Create(const char* type)
{
	MTF_Objects::Item* item = Search(type);
	if (!item)
		return NULL;
	return ((Create_t)(item->create))();
}

MTF_Objects::Item* MTF_Objects::Search(const char* type)
{
	for (auto& item : objects._items)
	{
		if (IsTypeEqual(item.type, type))
		{
			return &item;
		}
	}
	return NULL;
}

bool MTF_Objects::IsTypeEqual(const char* type0, const char* type1)
{
	mtf_i32 len0 = MTF_String::StrLen(type0);
	mtf_i32 len1 = MTF_String::StrLen(type1);
	mtf_i32 len = len0 < len1 ? len0 : len1;
	for (int32_t i = 0; i < len; i++)
	{
		if (type0[i] != type1[i])
			return false;
	}
	return true;
}
