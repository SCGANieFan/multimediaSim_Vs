#pragma once
#include <vector>
//#include "ObjectStatic.h"
#include "Allocator.h"
namespace smf {
	template<class T, class A = GlobleAllocator<T>>
	using vector = std::vector<T,A>;
}
