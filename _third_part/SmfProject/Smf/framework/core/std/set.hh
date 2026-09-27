#pragma once
#include <set>
#include "ObjectStatic.h"
#include "Allocator.h"
namespace smf {
	template<class Key_
		, class Compare_ = std::less<Key_>
		, class Allocator_ = GlobleAllocator<Key_>
	>
	using set = std::set<Key_, Compare_, Allocator_>;
	//
	template<class Key_
		, class Allocator_ = GlobleAllocator<Key_>
		, class Compare_ = std::less<Key_>
	>
	using set_t = std::set<Key_, Compare_, Allocator_>;
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
