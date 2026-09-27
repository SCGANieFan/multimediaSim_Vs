#pragma once
#include <map>
#include "ObjectStatic.h"
#include "Allocator.h"
namespace smf {
	template<class Key
		, class T
		, class Compare = std::less<Key>
		, class Allocator = GlobleAllocator<std::pair<const Key,T>>
	>
	using map = std::map<Key,T, Compare, Allocator>;
	//
	template<class Key
		, class T
		, class Allocator = GlobleAllocator<std::pair<const Key, T>>
		, class Compare = std::less<Key>
	>
	using map_t = std::map<Key, T, Compare, Allocator>;
	//
	//template<class Key
	//	, class Compare = std::less<Key>
	//	, class Allocator = GlobleAllocator<T >>
	//class set_x
	//	: public ObjectStatic
	//	, public std::set<Key, Compare, Allocator>
	//{
	//private:
	//	using Base = std::set<Key, Compare, Allocator>;
	//public:
	//	using Base::set;
	//	using Base::operator=;
	//};
}
