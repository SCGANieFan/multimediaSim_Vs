#pragma once
#include"MTF.Object.h"
#include"MTF.Memory.h"
#include<new>
using namespace mtf_ns;
class MTF_Objects
{
protected:
	typedef struct
	{
		mtf_void* create;
		const char* type;
	}Item;
public:
	MTF_Objects();
	~MTF_Objects();
public:
	template<class T>
	static bool Registe(const char* type)
	{
		for (auto& item : objects._items)
		{
			if (!item.create)
			{
				item.create = []() {
					mtf_void* buff = MTF_MALLOC(sizeof(T));
					return new(buff) T();
				};
				item.type = type;
				return true;
			}
			if (IsTypeEqual(item.type, type)) {
				item.create = []() {
					mtf_void* buff = MTF_MALLOC(sizeof(T));
					return new(buff) T();
				};
				return true;
			}
		}
		return false;
	}
	static mtf_void* Create(const char* type);

private:
	static Item* Search(const char* type);
	static bool IsTypeEqual(const char* type0, const char* type1);
private:
	static MTF_Objects objects;
	Item _items[10];
};

