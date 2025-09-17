#if 0
#include "MAF.Objects.h"

typedef void* (*Create_t)(void* buff);
typedef int32_t (*Getsize_t)(void);

MAF_Object MAF_Object::object;


MAF_Object::MAF_Object()
{
    memset(_items, 0, sizeof(_items));
}
MAF_Object::~MAF_Object()
{
}

int32_t MAF_Object::GetSize(const char* type)
{
	MAF_Object::Item* item = Search(type);
	if (!item)
		return 0;
	return ((Getsize_t)(item->getSize))();
}


void* MAF_Object::Create(const char* type, void* buff)
{
	MAF_Object::Item* item = Search(type);
	if(!item)
		return NULL;
	return ((Create_t)(item->create))(buff);
}

MAF_Object::Item* MAF_Object::Search(const char* type)
{
	uint64_t inType = *((uint64_t*)type);
	for (auto& item : object._items)
	{
		if (IsTypeEqual(item.type, type))
		{
			return &item;
		}
	}
	return NULL;
}

bool MAF_Object::IsTypeEqual(const char* type0, const char* type1)
{
	const char* ptr;
	int32_t len0 = 0;
	ptr = type0;
	if(!ptr)
		len0 = 0;
	else
	{
		while (*ptr++ != '\0')
		{
			len0++;
			if (len0 > 100)
				break;
		}
	}
	

	int32_t len1 = 0;
	ptr = type1;
	if (!ptr)
		len1 = 0;
	else
	{
		while (*ptr++ != '\0')
		{
			len1++;
			if (len1 > 100)
				break;
		}
	}

	if (len0 != len1)
		return false;

	for (int32_t i = 0; i < len0; i++)
	{
		if(type0[i] != type1[i])
			return false;
	}
	
	return true;
}

#endif