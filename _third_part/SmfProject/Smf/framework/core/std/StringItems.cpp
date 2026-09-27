#include "StringItems.h"
#include "SmfHash.h"
#include "SmfFCC.h"

using namespace smf;
StringItems::StringItems(Item_t* items, int cnt)
	:_items(items)
	, _cnt(cnt) 
{}
const char* StringItems::Find(uint32_t key) {
	//for (auto& item : _items) {
	for (int i = 0; i < _cnt; i++) {
		auto& item = _items[i];
		if (item.key == key)
			return item.str.c_str();
	}
	return 0;
}
bool StringItems::Add(uint32_t key, const char* val) {
	//for(auto&item:_items){
	for(int i=0;i<_cnt;i++){
		auto& item = _items[i];
		if (!item.key) {
			item.key = key;
			item.str = val;
			return true;
		}
	}
	return false;
}

