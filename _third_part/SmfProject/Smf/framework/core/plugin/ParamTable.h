#pragma once
#include "Object.h"
#include "SimpleTable.h"
namespace smf {
	template<class T, int C>
	class TParamTableC
		:public Object
	{
	public:
		using item_t = T;
		using table_t = SimpleTableC<T, C>;
	protected:
		table_t _table;
	protected:///params Set/Get
		virtual bool set(uint32_t key, void* val) override {
			switch (key) {
			case Hash("cfg"): return Deserialize((SimpleTable<void*>&) * this, (const char*)val);
			default: return Object::set(key, val) && val && _table.Set(key, setpara(*(item_t*)val));
			}
		}
		virtual bool get(uint32_t key, void* val) const override {
			return Object::get(key, val) && _table.get(key, *(item_t*)val);
		}
	protected:
		virtual item_t& setpara(item_t& item) { return item;  }
	public:
		table_t& Table() const { return (table_t&)_table; }
	};
}
