#pragma once
#include "SmfString.h"
#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif
namespace smf {
	template<class A, class B>
	constexpr A Max(A a, B b) {
		return (a > b) ? a : b;
	}
	template<class A, class B>
	constexpr A Min(A a, B b) {
		return (a < b) ? a : b;
	}
	template<class A,class Mi,class Ma>
	constexpr A Clamp(A a, Mi min, Ma max) {
		return Max(min,Min(max,a));
	}
}

