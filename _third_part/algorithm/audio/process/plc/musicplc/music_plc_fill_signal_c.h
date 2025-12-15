#pragma once
#include "plc_audio_samples_c.h"
#include "plc_base.h"
#include "plc_memory_manger_c.h"
class music_plc_fill_signal_c
{
public:
	music_plc_fill_signal_c() {};
	~music_plc_fill_signal_c() {};
public:
	INLINE void init(plc_memory_manger_c* mm,i32 fill_signal_sample,audio_info_c *info) {
		buffer_samples_c buffer_samples;
		buffer_samples._samples = fill_signal_sample;
		buffer_samples._buf = (u8*)mm->malloc(buffer_samples._samples * info->_bytes_per_sample);
		PLC_MEM_SET(buffer_samples._buf, 0, buffer_samples._samples * info->_bytes_per_sample);
		_fill_signal.init(info, &buffer_samples);
#if 0
		for (i16 ch = 0; ch < info->_channels; ch++)
			_fill_signal_sample_index[ch] = 0;
#else
		_fill_signal_sample_index[0] = 0;
#endif
	}
	
	INLINE void input(audio_samples_c& src, i32 src_sample, i32 append_sample, i16 ch) {
		_fill_signal.clear(_fill_signal.get_valid_samples(ch), ch);
		_fill_signal.append(
			src,
			src_sample,
			append_sample,
			ch);
		_fill_signal_sample_index[ch] = 0;
	}

	INLINE void input(audio_samples_c& src, i32 src_sample, i32 append_sample) {
		_fill_signal.clear(_fill_signal.get_valid_samples());
		_fill_signal.append(
			src,
			src_sample,
			append_sample);
		_fill_signal_sample_index[0] = 0;
	}

	INLINE void output(audio_samples_c& dst, i32 append_sample, i16 ch) {
		i32 append_sample_rem = append_sample;
		while (1) {
			i32 append_sample1 = _fill_signal.get_valid_samples(ch) - _fill_signal_sample_index[ch];
			append_sample1 = MIN(append_sample1, append_sample_rem);
			dst.append(
				_fill_signal,
				_fill_signal_sample_index[ch],
				append_sample1,
				ch);

			_fill_signal_sample_index[ch] = (_fill_signal_sample_index[ch] + append_sample1) % _fill_signal.get_valid_samples(ch);
			append_sample_rem = append_sample_rem - append_sample1;
			if (append_sample_rem <= 0)
				break;
		}
	}

	INLINE void output(audio_samples_c& dst, i32 append_sample){
		const int ch = 0;
		i32 append_sample_rem = append_sample;
		while (1) {
			i32 append_sample1 = _fill_signal.get_valid_samples() - _fill_signal_sample_index[ch];
			append_sample1 = MIN(append_sample1, append_sample_rem);
			dst.append(
				_fill_signal,
				_fill_signal_sample_index[ch],
				append_sample1);
			_fill_signal_sample_index[ch] = (_fill_signal_sample_index[ch] + append_sample1) % _fill_signal.get_valid_samples();
			append_sample_rem = append_sample_rem - append_sample1;
			if (append_sample_rem <= 0)
				break;
		}
	}
public:
	audio_samples_c _fill_signal;
	i32 _fill_signal_sample_index[1];
};
