#pragma once
#include "string.hh"
namespace smf {
	class StringItems{
	protected:
		struct Item_t {
			uint32_t key = 0;
			string str;
		};
	protected:
		Item_t* _items;
		const int _cnt;
	public:
		StringItems(Item_t* items, int cnt);
	public:
		const char* Find(uint32_t key);
		bool Add(uint32_t key, const char* val);
	};

	template<int C>
	class StringItemsC :public StringItems {
	public:
		StringItemsC() :StringItems(_items, C) {}
	private:
		Item_t _items[C];
	};
}
