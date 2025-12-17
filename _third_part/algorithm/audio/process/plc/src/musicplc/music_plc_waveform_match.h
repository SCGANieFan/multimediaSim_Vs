#pragma once

#include "music_plc_audio_data.h"

#if 1


enum class wave_form_match_func_mode_e {
	WAVE_FORM_MATCH_FUNC_MODE_SUM = 0,
	WAVE_FORM_MATCH_FUNC_MODE_ACCORELATION,
};

class wave_form_match_c
{
public:
	wave_form_match_c() {}
	~wave_form_match_c() {}
public:
	virtual void Init(wave_form_match_func_mode_e mode, audio_info_c* info) {
		_info = info;
	}
	virtual i32 do_wave_form_match_ch(void* ref, void* cmp, u16 ch_select, i32 seek_sample, i32 match_sample) = 0;
	virtual i32 do_wave_form_match_all_ch(void* ref, void* cmp, i32 seek_sample, i32 match_sample) = 0;
protected:
	audio_info_c* _info;

};

template<class Tref, class Tcmp, class Tnorm, int _rShitf1 = 0, int _rShitf0 = 0>
class wave_form_match_instance_c:public wave_form_match_c
{
public:
	class feature_c{
	public:
		feature_c() {}
		~feature_c() {}
		template<class T>
		void update(T* buf, i16 channels, i32 match_sample) {
			_match_sample = match_sample ? match_sample : 1;
			// T* buf_ori = buf;
			for (i32 ch = 0; ch < 1; ch++) {
			//for (i32 ch = 0; ch < channels; ch++) {
				T v = *buf;
				_v_abs_sum[ch] = abs(v);
				if (v > 0) {
					_v_p_sum[ch] = v;
					_v_n_sum[ch] = 0;
				}
				else {
					_v_p_sum[ch] = 0;
					_v_n_sum[ch] = v;
				}
				_v_sum[ch] = v;
				_v_dir[ch] = -v;
				buf += channels;
			}
			for (i32 m = 1; m < match_sample; m++) {
				for (i32 ch = 0; ch < 1; ch++) {
				//for (i32 ch = 0; ch < channels; ch++) {
					T v = *buf;
					_v_abs_sum[ch] += abs(v);
					if (v > 0) {
						_v_p_sum[ch] += v;
					}
					else {
						_v_n_sum[ch] += v;
					}
					_v_sum[ch] += v;
					if (m < match_sample >> 1) {
						_v_dir[ch] -= v;
					}
					else {
						_v_dir[ch] += v;
					}
					buf += channels;
				}
			}
		}
		template<class T>
		void update(T* buf, i16 channels) {
			T* buf_old = &buf[-channels * _match_sample];
			T* buf_old2 = &buf_old[channels * (_match_sample >> 1)];
			for (i32 ch = 0; ch < 1; ch++) {
			//for (i32 ch = 0; ch < channels; ch++) {
				T v = *buf;
				T v_old = *buf_old;
				T v_old2 = *buf_old2;
				_v_abs_sum[ch] -= abs(v_old);
				_v_abs_sum[ch] += abs(v);
				if (v_old > 0) {
					_v_p_sum[ch] -= v_old;
				}
				else {
					_v_n_sum[ch] -= v_old;
				}
				if (v > 0) {
					_v_p_sum[ch] += v;
				}
				else {
					_v_n_sum[ch] += v;
				}
				_v_sum[ch] -= v_old;
				_v_sum[ch] += v;
				_v_dir[ch] += v + v_old - (v_old2 * 2);
				++buf;
				++buf_old;
				++buf_old2;
			}
		}
		static inline Tnorm max(Tnorm x0, Tnorm x1) { return x0 > x1 ? x0 : x1; }
		static inline Tnorm min(Tnorm x0, Tnorm x1) { return x0 < x1 ? x0 : x1; }
		static inline Tnorm abs(Tnorm x) { return x > 0? x : -x; }
		static Tnorm evaluate(feature_c *ref, feature_c *cmp, i16 channels) {
			Tnorm scoreOut = 0;
			for (i32 ch = 0; ch < 1; ch++) {
			//for (i32 ch = 0; ch < channels; ch++) {
				Tnorm score = abs(ref->_v_abs_sum[ch] - cmp->_v_abs_sum[ch]);
				score += abs(ref->_v_p_sum[ch] - cmp->_v_p_sum[ch]);
				score += abs(ref->_v_n_sum[ch] - cmp->_v_n_sum[ch]);
				score += abs(ref->_v_sum[ch] - cmp->_v_sum[ch]);
				score += abs(ref->_v_dir[ch] - cmp->_v_dir[ch])*1;
				//score = score / ref->_match_sample;
				scoreOut += score;
			}
			return scoreOut;
		}

	public:
		constexpr static const i16 _channles = 8;
		Tnorm _v_abs_sum[_channles];
		Tnorm _v_p_sum[_channles];
		Tnorm _v_n_sum[_channles];
		Tnorm _v_sum[_channles];
		Tnorm _v_dir[_channles];
		i32 _match_sample;
	};

	static i32 wave_match_by_nf_run_ch(Tref* ref, Tcmp* cmp, i16 channels, u16 channel_select, i32 seek_sample, i32 match_sample) {
		return 0;
	}

	static i32 wave_match_by_nf_run_all_ch(Tref* ref, Tcmp* cmp, i16 channels, i32 seek_sample, i32 match_sample) {
		feature_c fea_ref;
		feature_c fea_cmp;
		Tref* p_ref = ref;
		Tcmp* p_cmp = cmp;
		// s=0
		fea_cmp.update(p_cmp, channels, match_sample);
		fea_ref.update(p_ref, channels, match_sample);
		Tnorm score_opt = feature_c::evaluate(&fea_ref, &fea_cmp, channels);
		i32 s_opt = 0;
		p_ref += channels * match_sample;
		const int stride = 3;
		for (i32 s = stride; s < seek_sample; s+= stride) {
			for (i32 str = 0; str < stride; str++) {
				fea_ref.update(p_ref, channels);
				p_ref += channels;
			}
			Tnorm score = feature_c::evaluate(&fea_ref, &fea_cmp, channels);
			if (score < score_opt) {
				score_opt = score;
				s_opt = s;
			}
		}
		return s_opt;
	}
public:
	wave_form_match_instance_c() {}
	~wave_form_match_instance_c() {}
public:
	virtual void Init(wave_form_match_func_mode_e mode, audio_info_c* info) override {
			_info = info;
	}
	virtual i32 do_wave_form_match_ch(void* ref, void* cmp, u16 ch_select, i32 seek_sample, i32 match_sample)override {
		return wave_match_by_nf_run_ch(
			(Tref*)ref,
			(Tcmp*)cmp,
			_info->_channels,
			ch_select,
			seek_sample,
			match_sample);
	}

	virtual i32 do_wave_form_match_all_ch(void* ref, void* cmp, i32 seek_sample, i32 match_sample) override {
		return wave_match_by_nf_run_all_ch(
			(Tref*)ref,
			(Tcmp*)cmp,
			_info->_channels,
			seek_sample,
			match_sample);
	}
};

#endif