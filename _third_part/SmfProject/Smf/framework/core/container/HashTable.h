#pragma once
#include "vector.hh"
#include "SmfHash.h"
#include "SmfDef.h"

namespace smf {
	template<class V>
	class HashTable :public vector<smf::Pair<uint32_t,V>> {
	private:
		typedef smf::Pair<uint32_t,V> Pair;
		typedef HashTable<V> Table;
		typedef typename vector<Pair>::iterator iterator;
		typedef typename vector<Pair>::const_iterator const_iterator;
	public:
		template<class K>
		V* Get(const K& key) const {
			auto iter = this->Find(key);
			return iter ? &iter->vals : 0;
		}
		template<class K>
		bool Set(const K& key, const V& val, bool add = true) {
			auto hash = Hash(key);
			auto iter = this->Find(hash);
			if (iter)iter->vals = val;
			else if (add)this->push_back({ hash,val });
			else return false;
			return true;
		}
		template<class K>
		bool Add(const K& key, const V& val, bool unique = true) {
			auto hash = Hash(key);
			if (unique) {
				if (this->Find(hash))
					return false;
			}
			this->push_back({ hash,val });
			return true;
		}
	public:
		iterator Find(uint32_t keys) {
			for (iterator iter = this->begin(); iter != this->end(); iter++)
				if (iter->keys == keys)
					return iter;
			return this->end();
		}
		const_iterator Find(uint32_t keys) const {
			for (const_iterator iter = this->begin(); iter != this->end(); iter++)
				if (iter->keys == keys)
					return iter;
			return this->end();
		}
	};

	template<class K, class V>
	class HashTable2 :public HashTable<smf::Pair<K, V>> {
	private:
		using Base = HashTable<smf::Pair<K, V>>;
		using Table = HashTable2<K, V>;
	public:
		V* Get(const K& key) const {
			auto x = Base::Get(key);
			return x ? &x->vals : 0;
		}
		bool Set(const K& key, const V& val, bool add = true) {
			auto hash = Hash(key);
			auto iter = this->Find(hash);
			if (iter)iter->vals.vals = val;
			else if (add)this->push_back({ hash,{key,val} });
			else return false;
			return true;
		}
		bool Add(const K& key, const V& val, bool unique = true) {
			auto hash = Hash(key);
			if (unique) {
				if (this->Find(hash))
					return false;
			}
			this->push_back({ hash,{key,val} });
			return true;
		}
	};
}
