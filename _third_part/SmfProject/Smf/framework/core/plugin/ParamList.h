#pragma once
#include "Object.h"
#include "SimpleTable.h"
namespace smf {
	class ParamList
		: public Object
		//, public SimpleTable<void*>
	{
	public:
		using item_t = StringTable::item_t;
	public:
		ParamList(item_t* items, uint32_t count);
	protected:///params Set/Get
		virtual bool set(uint32_t key, void* val);
		virtual bool get(uint32_t key, void* val) const;
	protected:
		StringTable _table;
	public:
		StringTable& Table() const { return (StringTable&)_table; }
	};

	template<int C>
	class ParamListC : public ParamList {
	public:
		ParamListC() :ParamList(_items,C) {}
	private:
		item_t _items[C];
	};
}
