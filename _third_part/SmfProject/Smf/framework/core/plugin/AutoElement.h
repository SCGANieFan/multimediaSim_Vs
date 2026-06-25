#pragma once
#include "Element.h"
#include "string.hh"
namespace smf {
	class AutoElement
	{
	public:
		using CbBeforeReplace_t = bool (*)(const char* types, const char* keys);
	public:
		AutoElement(Element*this_);
		~AutoElement();
	protected:
		Element* _this = 0;
		uint64_t _keys = 0;
		string _types;
		void* _enc = 0;
		uint16_t _sidx = 0;
		uint16_t _gidx = 0;
		mutable smf_keys_value_t _items[32];
		mutable smf_keys_value_t _items_get[8];
	protected:
		CbBeforeReplace_t _cbBeforeReplace = 0;
	protected:
		Result set(uint32_t key, void* val);
		Result get(uint32_t key, void* val) const;
		bool open(uint64_t keys);
		bool duplicate(uint32_t key, void* val, void*& dst, int size);
	protected:
		friend class AutoSource;
		friend class AutoSink;
		friend class AutoFilter;
	};
}