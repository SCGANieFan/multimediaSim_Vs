#if 0
#pragma once
#include<new>

class MAF_Object
{
protected:
    typedef struct
    {
        void* create;
        void* getSize;
        const char *type;
    }Item;
public:
	MAF_Object();
	~MAF_Object();

public:
	template<class T>
    static bool Registe(const char* type)
	{
		for (auto& item : object._items)
		{
			if (IsTypeEqual(item.type, type)) {
				item.create = [](void* buff) {return new(buff) T(); };
				item.getSize = []() {return sizeof(T); };
				return true;
			}

			if (!item.create)
			{
				item.create = [](void* buff) {return new(buff) T(); };
				item.type = type;
				item.getSize = []() {return sizeof(T); };
				return true;
			}
		}
		return false;
	}

    static int32_t GetSize(const char* type);
    static void* Create(const char* type, void* buff);


private:
	static Item *Search(const char* type);
	static bool IsTypeEqual(const char* type0, const char* type1);
private:
	static MAF_Object object;
	Item _items[10];

};

#endif