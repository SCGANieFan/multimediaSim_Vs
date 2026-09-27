#pragma once
#include <list>
#include "ObjectStatic.h"
#include "Allocator.h"
namespace smf {
	template<class T, class A = GlobleAllocator<T>>
	using list = std::list<T,A>;
	//
	//template<class T, class A>
	//class list_x
	//	: public ObjectStatic
	//	, public std::list<T, A>
	//{
	//private:
	//	using Base = std::list<T, IAllocator<T>>;
	//public:
	//	using Base::list;
	//	using Base::operator=;
	//};
}
