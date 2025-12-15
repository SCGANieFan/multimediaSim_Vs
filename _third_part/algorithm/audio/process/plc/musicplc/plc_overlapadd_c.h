#if 1
#pragma once
#include "plc_audio_cal_com.h"
#include "plc_audio_cal_buffer_generate_c.h"
#include "plc_base.h"
#include "plc_audio_data_c.h"

template<class Ti, class To, class Tf>
struct overlap_add_t {
	STATIC b1 run_ch(void* dst, void* src_decline, void* src_rise, const i16 channel_select, const i16 channels, void* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample) {
		const i32 fixNum = 13;
		Ti* pSrcRise = (Ti*)src_rise + channel_select;
		Ti* pSrcDecline = (Ti*)src_decline + channel_select;
		To* p_dst = (To*)dst + channel_select;
		Tf* pFac = (Tf*)factorDecLine;
		for (i32 s = startOverlapSample; s < endOverlapSample; s++) {
			*p_dst = (To)(((i64)(*pSrcRise) * pFac[overlapSample - s - 1] + (i64)(*pSrcDecline) * pFac[s]) >> fixNum);
			p_dst += channels;
			pSrcRise += channels;
			pSrcDecline += channels;
		}
		return true;
	}
	STATIC b1 run_all_ch(void* dst, void* src_decline, void* src_rise, const i16 channels, void* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample) {
		const i32 fixNum = 13;
		Ti* pSrcRise = (Ti*)src_rise;
		Ti* pSrcDecline = (Ti*)src_decline;
		To* p_dst = (To*)dst;
		Tf* pFac = (Tf*)factorDecLine;
		for (i32 s = startOverlapSample; s < endOverlapSample; s++) {
			for (i16 ch = 0; ch < channels; ch++) {
				*p_dst = (To)(((i64)(*pSrcRise) * pFac[overlapSample - s - 1] + (i64)(*pSrcDecline) * pFac[s]) >> fixNum);
				p_dst++;
				pSrcRise++;
				pSrcDecline++;
			}
		}
		return true;
	}
};

template<>
struct overlap_add_t<f32, f32, f32> {
	STATIC b1 run_ch(void* dst, void* src_decline, void* src_rise, const i16 channel_select, const i16 channels, void* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample) {
		f32* pSrcRise = (f32*)src_rise + channel_select;
		f32* pSrcDecline = (f32*)src_decline + channel_select;
		f32* p_dst = (f32*)dst + channel_select;
		f32* pFac = (f32*)factorDecLine;
		for (i32 s = startOverlapSample; s < endOverlapSample; s++) {
			*p_dst = (*pSrcRise) * pFac[overlapSample - s - 1] + (*pSrcDecline) * pFac[s];
			p_dst += channels;
			pSrcRise += channels;
			pSrcDecline += channels;
		}
		return true;
	}
	STATIC b1 run_all_ch(void* dst, void* src_decline, void* src_rise, const i16 channels, void* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample) {
		f32* pSrcRise = (f32*)src_rise;
		f32* pSrcDecline = (f32*)src_decline;
		f32* p_dst = (f32*)dst;
		f32* pFac = (f32*)factorDecLine;
		for (i32 s = startOverlapSample; s < endOverlapSample; s++) {
			for (i16 ch = 0; ch < channels; ch++) {
				*p_dst = (*pSrcRise) * pFac[overlapSample - s - 1] + (*pSrcDecline) * pFac[s];
				p_dst++;
				pSrcRise++;
				pSrcDecline++;
			}
		}
		return true;
	}
};


enum class overlap_add_window_choose_e {
	Line = 0,
	Cosine,
};

template<class T, b1 type = IsFloat<T>()>
struct overlap_add_inner_t : overlap_add_t<T, T, T> {};
template<class T>
struct overlap_add_inner_t<T, false> : overlap_add_t<T, T, i32> {};

template<class T>
class overlap_add_c
{
public:
	overlap_add_c() {};
	~overlap_add_c() {};
public:
	INLINE b1 is_finish(i16 ch) { return  b1(_overlap_samples_now[ch] >= _overlap_samples); }
	INLINE b1 is_finish() { return  b1(_overlap_samples_now[0] >= _overlap_samples); }
	INLINE void start(i16 ch) {
		_overlap_samples_now[ch] = 0;
	}
	INLINE void start() {
		_overlap_samples_now[0] = 0;
	}

public:
	INLINE b1 init(plc_memory_manger_c* mm, audio_info_c* info, overlap_add_window_choose_e window_choose, i32 overlap_samples) {
		_info = info;
		_overlap_samples = overlap_samples;
#if 0
		for (i16 ch = 0; ch < _info->_channels; ch++) {
			_overlap_samples_now[ch] = _overlap_samples;
		}
#else
		_overlap_samples_now[0] = _overlap_samples;
#endif
		buffer_generator_c::buffer_choose_e buffer_choose;
		if (window_choose == overlap_add_window_choose_e::Line)
			buffer_choose = buffer_generator_c::buffer_choose_e::WINDOW_LINE_FADE;
		else if (window_choose == overlap_add_window_choose_e::Cosine)
			buffer_choose = buffer_generator_c::buffer_choose_e::WINDOW_COSINE_FADE;
		else {
			buffer_choose = buffer_generator_c::buffer_choose_e::WINDOW_COSINE_FADE;
		}
		if (IsF32<T>()) {
			_factor = mm->malloc(_overlap_samples * sizeof(f32));//1->0
			_fix_num = 0;
			buffer_generator_c::generate(buffer_choose, (f32*)_factor, _overlap_samples, _fix_num);
		}
		else {
			_factor = mm->malloc(_overlap_samples * sizeof(i32));//1->0
			_fix_num = 13;
			buffer_generator_c::generate(buffer_choose, (i32*)_factor, _overlap_samples, _fix_num);
		}
		return true;
	}

	INLINE void do_overlap_add(void* dst, void* src_decline, void* src_rise, i32 do_overlap_samples, i16 ch) {
		i32 do_overlap_samples0 = _overlap_samples - _overlap_samples_now[ch];
		do_overlap_samples0 = MIN(do_overlap_samples0, do_overlap_samples);
#if 1
		overlap_add_inner_t<T>::RunCh(
			dst,
			src_decline,
			src_rise,
			ch,
			_info->_channels,
			_factor,
			_overlap_samples_now[ch],
			_overlap_samples_now[ch] + do_overlap_samples0,
			_overlap_samples);
#endif
#if 1
		if (do_overlap_samples > do_overlap_samples0) {
#if 0
			MUSIC_PLC_MEM_CPY(
				((u8*)dst + do_overlap_samples0 * _info->_bytes_per_sample),
				((u8*)src_rise + do_overlap_samples0 * _info->_bytes_per_sample),
				(do_overlap_samples - do_overlap_samples0) * _info->_bytes_per_sample);
#else
			T* p_src = ((T*)src_rise + do_overlap_samples0 * _info->_channels);
			T* p_dst = ((T*)dst + do_overlap_samples0 * _info->_channels);
			for (i32 s = 0; s < do_overlap_samples - do_overlap_samples0; s++) {
				*p_dst = *p_src;
				//*p_dst = 0;
				p_dst += _info->_channels;
				p_src += _info->_channels;
			}
#endif
		}
#endif
		_overlap_samples_now[ch] += do_overlap_samples0;
	}


	INLINE void do_overlap_add(void* dst, void* src_decline, void* src_rise, i32 do_overlap_samples) {
		i32 do_overlap_samples0 = _overlap_samples - _overlap_samples_now[0];
		do_overlap_samples0 = MIN(do_overlap_samples0, do_overlap_samples);
#if 1
		overlap_add_inner_t<T>::run_all_ch(
			dst,
			src_decline,
			src_rise,
			_info->_channels,
			_factor,
			_overlap_samples_now[0],
			_overlap_samples_now[0] + do_overlap_samples0,
			_overlap_samples);
#endif
		if (do_overlap_samples > do_overlap_samples0) {
			PLC_MEM_CPY(
				((u8*)dst + do_overlap_samples0 * _info->_bytes_per_sample),
				((u8*)src_rise + do_overlap_samples0 * _info->_bytes_per_sample),
				(do_overlap_samples - do_overlap_samples0) * _info->_bytes_per_sample);
		}
		_overlap_samples_now[0] += do_overlap_samples0;
	}
private:
	audio_info_c* _info;
	i32 _overlap_samples;
	i32 _overlap_samples_now[1];
	void* _factor;
	i32 _fix_num = 15;
};
#endif