#if 1
#pragma once

template<class T>
struct buffer_generator_t {
	STATIC INLINE void generate_line(T* factor, i32 samples, i16 fp_num = 0) {
		i64 samples_inner = samples + 1;
		for (i32 i = 0; i < samples; i++) {
			i64 num = samples_inner - 1 - i;
			factor[i] = (T)((num << fp_num) / samples_inner);
		}
	}
	STATIC INLINE void generate_cos(T* factor, i32 samples, i16 fp_num = 0) {
		STATIC const i32 _PI_Q15 = 102943;
		STATIC const i32 _0p1_Q15 = 690;
		i32 PI_QFp = _PI_Q15 >> (15 - fp_num);
		i32 _0P1_QFp = _0p1_Q15 >> (15 - fp_num);
		i32 ONE_QFp = ((i32)1 << fp_num);
		i32 ONE_QFpP1 = ((i32)1 << (fp_num + 1));
		for (i32 i = 0; i < (samples + 1) / 2; i++) {
			i32 xQFp = PI_QFp * i / samples;
			i32 offset = _0P1_QFp * i / samples;
			i32 xHat2QFpPFp = xQFp * xQFp;
			i32 xHat2QFpP2 = (xHat2QFpPFp + ONE_QFpP1) >> (fp_num + 2);
			i32 xHat2QFp = (xQFp * xQFp) >> fp_num;
			i32 xHat4QFp = (xHat2QFp * xHat2QFp) >> fp_num;
			factor[i] = (T)(ONE_QFp - xHat2QFpP2 + xHat4QFp / 48 - offset);
		}
		i32 xx = ((samples + 1) / 2 * 2) - 1;
		for (i32 i = (samples + 1) / 2; i < samples; i++)
		{
			factor[i] = ONE_QFp - factor[xx - i];
		}
	}
};

template<>
struct buffer_generator_t<f32> {
	STATIC INLINE void generate_line(f32* factor, i32 samples, i16 fp_num = 0) {
		i32 samples_inner = samples + 1;
		for (i32 i = 0; i < samples; i++) {
			f32 num = samples_inner - 1 - i;
			factor[i] = num / samples_inner;
		}
	}
	STATIC INLINE void generate_cos(f32* factor, i32 samples, i16 fp_num = 0) {
		STATIC const f32 _PI = 3.1415926f;
		for (i32 i = 0; i < (samples + 1) / 2; i++) {
			f32 x = _PI * i / samples;
			f32 xHat2 = x * x;
			f32 xHat2D4 = xHat2 / 4;
			f32 xHat4D48 = xHat2 * xHat2 / 48;
			factor[i] = (f32)((f32)1 - xHat2D4 + xHat4D48);
		}
		i32 xx = ((samples + 1) / 2 * 2) - 1;
		for (i32 i = (samples + 1) / 2; i < samples; i++)
		{
			factor[i] = 1 - factor[xx - i];
		}
	}
};

class buffer_generator_c
{
public:
	enum class buffer_choose_e {
		WINDOW_LINE_FADE = 0,
		WINDOW_COSINE_FADE,
	};
public:
	buffer_generator_c() {};
	~buffer_generator_c() {};
public:
	template<class T>
	STATIC INLINE void generate(buffer_choose_e choose, T* factor, i32 samples, i16 fp_num = 0) {
		fp_num = IsFloat<T>() ? 0 : MIN(fp_num, 15);
		if (choose == buffer_choose_e::WINDOW_LINE_FADE) {
			buffer_generator_t<T>::generate_line(factor, samples, fp_num);
		}
		else if (choose == buffer_choose_e::WINDOW_COSINE_FADE) {
			buffer_generator_t<T>::generate_cos(factor, samples, fp_num);
		}
	}
};

#endif