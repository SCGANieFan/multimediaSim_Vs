#pragma once
#include "SmfHash.h"
#include "SmfDef.h"
#include "SmfString.h"

namespace smf {
	template<class V>
	class SimpleArray {
	public:
		using value_t = V;
		using item_t = V;
	protected:
		V* _items = 0;
		uint32_t _cnt = 0;
	public:
		SimpleArray(V*items, uint32_t cnt):_items(items),_cnt(cnt){
		}
	public:
		bool Get(uint32_t idx, V& val) const {
			if (idx > _cnt)return false;
			val = _items[idx];
			return true;
		}
		bool Get(uint32_t idx, V* val) const {
			if (idx > _cnt)return false;
			val = &_items[idx];
			return true;
		}
		V* Get(uint32_t idx) const {
			if (idx > _cnt)return 0;
			return (V*)&_items[idx];
		}
		bool Set(uint32_t idx, const V& val) {
			if (idx > _cnt)return false;
			_items[idx] = (V&)val;
			return true;
		}
	public:
		uint32_t Count()const { 
			return _cnt; 
		}
		V& operator[](int idx) {
			return _items[idx];
		}
		const V& operator[](int idx) const {
			return _items[idx];
		}
	public:
		V* begin() {
			return _items;
		}
		V* end() {
			return _items + _cnt;
		}
		const V* begin() const {
			return _items;
		}
		const V* end() const {
			return _items + _cnt;
		}
	};

	template<class V, int C>
	class SimpleArrayC : public SimpleArray<V> {
	private:
		V _array[C];
	public:
		SimpleArrayC():SimpleArray<V>(_array,C){}
	};

	bool Deserialize(SimpleArray<const char*>&, const char* script);
}
