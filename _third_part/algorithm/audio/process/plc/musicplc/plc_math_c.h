#pragma once
#include "plc_base.h"

#define ALGO_ABS(x) ((x)>0?(x):(-x))


#if 1
	template<class Ti, class To>
	struct sqrt_t {
		static inline To run(Ti x) {
			if (x == 0)
				return 0;
			Ti left = 1;
			Ti right = x;
			while (left <= right) {
				Ti mid = left + ((right - left) >> 1);
				if (x >= (mid * mid)) {
					if (x < (mid + 1) * (mid + 1)) {
						return mid;
					}
					else {
						left = mid + 1;
					}
				}
				else {
					right = mid - 1;
				}
			}
			return 0;
		}
	};

	template<>
	struct sqrt_t<f32, f32> {
		static inline f32 run(f32 a) {
			if (!IsFloatValid(a)) {
				return 0;
			}
			if (a < 0) {
				return 0;
			}
			static const f32 EPSILON = 0.00001f;
			const i32 loop_num_max = 100;
			i32 loop_num = 0;
			f32 x = a / 2.0f;
			f32 y = x;
			f32 diff;
			while (1) {
				y = x;
				x = 0.5f * (x + a / x);
				diff = x - y;
				if (ALGO_ABS(diff) < EPSILON
					|| loop_num > loop_num_max) {
					break;
				}
				loop_num++;
			}
			return x;
		}
	};
#endif

#if 1
	template<class Ti, class To>
	struct division_t {
		STATIC INLINE To run(Ti dividend, Ti divisor) {
#if 0
			if (divisor == 0) return -1;
			if (dividend == 0) return 0;
			if (dividend < divisor) return 0;

			i64 result = 0;
			i64 temp_divisor = divisor;
			i64 multiple = 1;
			while (dividend >= (temp_divisor << 1)) {
				temp_divisor <<= 1;
				multiple <<= 1;
			}

			while (dividend >= divisor) {
				if (dividend >= temp_divisor) {
					dividend -= temp_divisor;
					result += multiple;
				}
				multiple >>= 1;
				temp_divisor >>= 1;
			}

			return result;
#else
			return (To)(dividend / divisor);
#endif
		}
	};

	template<>
	struct division_t<f32, f32> {
		STATIC INLINE f32 run(f32 dividend, f32 divisor) {
			return dividend / divisor;
		}
	};
#endif


#if 1
template<class T>
class math_c
{
public:
	math_c() {}
	~math_c() {}
public:
	STATIC INLINE T sqrt(T x) {
		return sqrt_t<T, T>::run(x);
	}
	STATIC INLINE T division(T dividend, T divisor) {
		return division_t<T, T>::run(dividend, divisor);
	}
};
#endif


