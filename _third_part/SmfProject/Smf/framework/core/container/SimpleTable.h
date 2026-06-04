#pragma once
#include "SmfHash.h"

namespace smf {
	template<class V>
	class SimpleTable {
	public:
		using value_t = V;
		struct item_t {
			uint32_t keys;
			V vals;
		};
	protected:
		item_t* _items = 0;
		int _cnt = 0;
	public:
		SimpleTable(item_t*items,int cnt) {
			_cnt = cnt;
			_items = items;
			for (int i = 0; i < _cnt; i++) {
				auto& item = _items[i];
				item.keys = 0;
			}
		}
		template<class K>
		bool Get(const K& key,V& val) const {
			auto iter = this->Find(key);
			if (iter)val = iter->vals;
			return iter;
		}
		template<class K>
		bool Get(const K& key, V* val) const {
			auto iter = this->Find(key);
			if (iter)val = &iter->vals;
			return iter;
		}
		template<class K>
		V* Get(const K& key) const {
			auto iter = this->Find(key);
			return iter ?(V*)&iter->vals : 0;
		}
		template<class K>
		bool Set(const K& key, const V& val, bool add = true) {
			auto hash = Hash(key);
			auto iter = this->Find(hash);
			if (iter) {
				iter->vals = val;
				return true;
			}
			else if (add) {
				iter = this->Find(0u);
				if (iter) {
					iter->keys = hash;
					iter->vals = val;
					return true;
				}
			}
			return false;
		}
		template<class K>
		bool Remove(const K& key) {
			auto hash = Hash(key);
			auto iter = this->Find(hash);
			if (iter) {
				iter->keys = 0;
			}
			return true;
		}
	public:
		uint32_t Count()const {
			return _cnt;
		}
		item_t& operator[](int idx) {
			return _items[idx];
		}
		const item_t& operator[](int idx) const {
			return _items[idx];
		}
	public:
		template<class K>
		item_t* Find(const K& keys) const {
			auto hash = Hash(keys);
			//for (auto& item : this->_items) {
			for(int i=0;i<_cnt;i++){
				auto& item = _items[i];
				if (item.keys == hash)
					return (item_t*)&item;
			}
			return 0;
		}
	public:
		item_t* begin() {
			return _items;
		}
		item_t* end() {
			return _items + _cnt;
		}
		const item_t* begin() const {
			return _items;
		}
		const item_t* end() const {
			return _items + _cnt;
		}
	public:
		item_t* Last() { return _idx < _cnt ? &_items[_idx] : 0; }
		bool Append() { return _idx < _cnt ? _idx++ : 0; }
		bool Append(uint32_t keys, const V& vals) {
			auto item = Last();
			if (item) {
				item->keys = keys;
				item->vals = vals;
			}
			return Append();
		}
		bool Append(const char* keys, const V& vals) { return Append(Hash(keys), vals);}
	private:
		int _idx = 0;
	};

	using StringTable = SimpleTable<const char*>;

	template<class V, int C>
	class SimpleTableC : public SimpleTable<V> {
	private:
		typename SimpleTable<V>::item_t _items[C];
	public:
		SimpleTableC():SimpleTable<V>(_items,C){}
	};

	bool Deserialize(SimpleTable<const char*>&this_, const char* script);
	bool Deserialize(SimpleTable<void*>&this_, const char* script, uint32_t* params = 0);
}
