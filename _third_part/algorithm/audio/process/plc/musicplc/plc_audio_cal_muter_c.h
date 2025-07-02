#if 1
#pragma once
#include "plc_audio_cal_com.h"
#include "plc_audio_cal_product_c.h"
#include "plc_audio_cal_buffer_generate_c.h"
#include "plc_base.h"
#include "plc_audio_data_c.h"
#include "plc_math_c.h"

enum class muter_window_choose_e {
	MUTER_WINDOW_CHOOSE_LINE=0,
	MUTER_WINDOW_CHOOSE_COSINE,
};
enum class muter_dir_choose_e {
	MUTER_DIR_CHOOSE_ATTENUATION = 0,
	MUTER_DIR_CHOOSE_AMPLIFICATION,
};

#if 1
template<class T, i8 type=(IsI16<T>() ? 0 : (IsI32<T>() ? 1 : 2))>
class MuterProduct_c : public Product_c<T, T, T, T> {};
template<class T>
class MuterProduct_c<T, 0> : public Product_c<T, T, i16, i32, 14> {};
template<class T>
class MuterProduct_c<T, 1> : public Product_c<T, T, i16, i64, 14> {};
//template<class T>
//class MuterProduct_c<T, 1> : public Product_c<T, T, T, T> {};
#endif

template<class T>
class muter_c
{
public:
	muter_c() {}
	~muter_c() {}
public:
	void init(plc_memory_manger_c* mm, audio_info_c* info, muter_window_choose_e window_choose, muter_dir_choose_e dir_choose, i32 decay_samples) {
		_info = info;
		_mute_samples_max = decay_samples;
		for (i16 ch = 0; ch < info->_channels; ch++)
			reset(dir_choose, ch);
		buffer_generator_c::buffer_choose_e buffer_choose = buffer_generator_c::buffer_choose_e::WINDOW_LINE_FADE;
		if (window_choose == muter_window_choose_e::MUTER_WINDOW_CHOOSE_LINE)
			buffer_choose = buffer_generator_c::buffer_choose_e::WINDOW_LINE_FADE;
		else if (window_choose == muter_window_choose_e::MUTER_WINDOW_CHOOSE_COSINE)
			buffer_choose = buffer_generator_c::buffer_choose_e::WINDOW_COSINE_FADE;
		if(IsFloat<T>()){
			_mute_factor_width = sizeof(T);
			_mute_factor = (u8*)mm->malloc(decay_samples * _mute_factor_width);
			PLC_MEM_SET(_mute_factor, 0, decay_samples * _mute_factor_width);
			buffer_generator_c::generate<T>(buffer_choose, (T*)_mute_factor, _mute_samples_max);
		}
		else {
			_facFpNum = 14;
			_mute_factor_width = sizeof(i16);
			_mute_factor = (u8*)mm->malloc(decay_samples * _mute_factor_width);
			PLC_MEM_SET(_mute_factor, 0, decay_samples * _mute_factor_width);
			buffer_generator_c::generate<i16>(buffer_choose, (i16*)_mute_factor, _mute_samples_max, _facFpNum);
		}
	}
	INLINE void setDir(muter_dir_choose_e dir, i16 ch) { _dir[ch] = dir; }
	INLINE void setDir(muter_dir_choose_e dir) { setDir(dir, 0); }
	INLINE void sync(muter_c dst, i16 ch) {
		//_muteSamplesNow = dst._muteSamplesNow * _mute_samples_max / dst._mute_samples_max;
		_muteSamplesNow[ch] = math_c<i32>::division(dst._muteSamplesNow[ch] * _mute_samples_max, dst._mute_samples_max);
	}
	INLINE void sync(muter_c dst) {
		sync(dst, 0);
	}
	INLINE void reset(muter_dir_choose_e dir, i16 ch) {
		_dir[ch] = dir;
		if (_dir[ch] == muter_dir_choose_e::MUTER_DIR_CHOOSE_ATTENUATION)
			_muteSamplesNow[ch] = 0;
		else if (_dir[ch] == muter_dir_choose_e::MUTER_DIR_CHOOSE_AMPLIFICATION)
			_muteSamplesNow[ch] = _mute_samples_max;
	}
	INLINE void reset(muter_dir_choose_e dir) {
		reset(dir, 0);
	}
	INLINE b1 is_mute_finish(i16 ch) {
		if (_dir[ch] == muter_dir_choose_e::MUTER_DIR_CHOOSE_ATTENUATION)
			return (b1)(_muteSamplesNow[ch] >= _mute_samples_max);
		else if (_dir[ch] == muter_dir_choose_e::MUTER_DIR_CHOOSE_AMPLIFICATION)
			return (b1)(_muteSamplesNow[ch] <= 0);
		return false;
	}
	INLINE b1 is_mute_finish() {
		return is_mute_finish(0);
	}

	INLINE void doMute(void* src, const i32 productSample, i16 ch) {
#if 1
#if 0
		{static int num = 0; 
			printf("(%s)[%d]<%d> %d,%d,%d\n", __func__, __LINE__, num++,
				ch, _dir[ch],_muteSamplesNow[ch]);
		}
#endif
		if (_dir[ch] == muter_dir_choose_e::MUTER_DIR_CHOOSE_ATTENUATION) {
			i32 doMuteSamples = _mute_samples_max - _muteSamplesNow[ch];
			doMuteSamples = MIN(doMuteSamples, productSample);			
			MuterProduct_c<T>::RunChForWard(
				src,
				src,
				&_mute_factor[_mute_factor_width * _muteSamplesNow[ch]],
				doMuteSamples,
				_info->_channels);
			if (doMuteSamples < productSample) {
				PLC_MEM_SET(
					((u8*)src + doMuteSamples * _info->_bytes_per_sample),
					0,
					(productSample - doMuteSamples) * _info->_bytes_per_sample);
			}
			_muteSamplesNow[ch] += productSample;
			_muteSamplesNow[ch] = MIN(_muteSamplesNow[ch], _mute_samples_max);
		}
		else {
			i32 doMuteSamples = _muteSamplesNow[ch];
			doMuteSamples = MIN(doMuteSamples, productSample);
			MuterProduct_c<T>::RunChBackWard(
				src,
				src,
				&_mute_factor[_mute_factor_width * (_muteSamplesNow[ch] - 1)],
				doMuteSamples,
				_info->_channels);
			_muteSamplesNow[ch] -= productSample;
			_muteSamplesNow[ch] = MAX(_muteSamplesNow[ch], 0);
		}
#endif
	}

	INLINE void doMute(void* src, const i32 productSample) {
#if 1
		if (_dir[0] == muter_dir_choose_e::MUTER_DIR_CHOOSE_ATTENUATION) {
			i32 doMuteSamples = _mute_samples_max - _muteSamplesNow[0];
			doMuteSamples = MIN(doMuteSamples, productSample);
			MuterProduct_c<T>::RunAllChForWard(
				src,
				src,
				&_mute_factor[_mute_factor_width * _muteSamplesNow[0]],
				doMuteSamples,
				_info->_channels);
			if (doMuteSamples < productSample) {
				PLC_MEM_SET(
					((u8*)src + doMuteSamples * _info->_bytes_per_sample),
					0,
					(productSample - doMuteSamples) * _info->_bytes_per_sample);
			}
			_muteSamplesNow[0] += productSample;
			_muteSamplesNow[0] = MIN(_muteSamplesNow[0], _mute_samples_max);
		}
		else {
			i32 doMuteSamples = _muteSamplesNow[0];
			doMuteSamples = MIN(doMuteSamples, productSample);
			MuterProduct_c<T>::RunAllChBackWard(src,
				src,
				&_mute_factor[_mute_factor_width * (_muteSamplesNow[0] - 1)],
				doMuteSamples,
				_info->_channels);
			_muteSamplesNow[0] -= productSample;
			_muteSamplesNow[0] = MAX(_muteSamplesNow[0], 0);
		}
#endif
	}

private:
	u8* _mute_factor;
	i32 _mute_factor_width;
	i32 _muteSamplesNow[16];
	i32 _mute_samples_max;
	muter_dir_choose_e _dir[16];
	i8 _facFpNum;
	audio_info_c* _info;
};

#endif