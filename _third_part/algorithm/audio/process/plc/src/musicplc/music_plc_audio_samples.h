#pragma once
#include "music_plc_audio_data.h"

class buffer_samples_c
{
public:
	buffer_samples_c() {};
	~buffer_samples_c() {};
public:
	u8* _buf = 0;
	i32 _samples = 0;
};

class audio_samples_c :public audio_data_c
{
private:
	class channel_info_c {
	public:
		channel_info_c() {};
		~channel_info_c() {};
	public:
		i32 _used_sample = 0;
		i32 _valid_samples = 0;
	};
public:
	audio_samples_c() {};
	~audio_samples_c() {};
public:
	//set

	//get
	INLINE u8* get_left_data(i16 ch) { 
		return &_buff[(_channel_infos[ch]._used_sample + _channel_infos[ch]._valid_samples) * _info->_bytes_per_sample + ch * _info->_width];
	}
	INLINE u8* get_left_data() {
		return get_left_data(0);
	}

	INLINE u8* get_buf_in_sample(i32 sample, i16 ch) { return &_buff[sample * _info->_bytes_per_sample + ch * _info->_width]; }
	INLINE u8* get_buf_in_sample(i32 sample) { return get_buf_in_sample(sample, 0); }

	INLINE i32 get_valid_samples(i16 ch) { return _channel_infos[ch]._valid_samples; }
	INLINE i32 get_valid_samples() { return get_valid_samples(0); }

	INLINE i32 get_used_samples(i16 ch) { return _channel_infos[ch]._used_sample;}
	INLINE i32 get_used_samples() { return get_used_samples(0); }

	INLINE i32 get_samples_max() { return _samples; };

	INLINE i32 get_left_samples(i16 ch) { return _samples - get_used_samples(ch) - get_valid_samples(ch); }
	INLINE i32 get_left_samples() { return get_left_samples(0); }

	INLINE i32 get_size_max() { return _samples_total * _info->_width; };
	INLINE i16 get_fp_num() { return _fp_num; };
	INLINE b1 is_full() { return get_valid_samples() == get_samples_max(); };

	//
	INLINE b1 init(const audio_info_c* p_info, buffer_c* buffer, i16 fp_num = 0) {
		_samples = buffer->_max / p_info->_bytes_per_sample;
		// _valid_samples = 0;
		_samples_total = _samples * p_info->_channels;
		_fp_num = fp_num;
		return audio_data_c::init(p_info, buffer);
	}

	INLINE b1 init(const audio_info_c* p_info, buffer_samples_c* buffer_samples, i16 fp_num = 0) {
		_samples = buffer_samples->_samples;
		// _valid_samples = 0;
		_samples_total = _samples * p_info->_channels;
		_fp_num = fp_num;

		buffer_c buffer;
		buffer._buf = buffer_samples->_buf;
		buffer._max = _samples * p_info->_bytes_per_sample;
		return audio_data_c::init(p_info, &buffer);
	}
	INLINE b1 append(audio_samples_c& src, i32 srcSample, i32 appendSample, i16 ch) {
#if 0
		i32 copyByte = appendSample * _info->_bytes_per_sample;
		PLC_MEM_CPY(get_left_data(), src.get_buf_in_sample(srcSample), copyByte);
		_valid_samples += appendSample;
		_size += copyByte;
		return true;
#else
		i32 stride = _info->_channels;
		if (_info->_width == 2) {
			i16* p_dst = (i16*)get_left_data(ch);
			i16* p_src = (i16*)src.get_buf_in_sample(srcSample, ch);
			for (i32 s = 0; s < appendSample; s++) {
				*p_dst = *p_src;
				p_dst += stride;
				p_src += stride;
			}
		}
		else if (_info->_width == 4) {
			i32* p_dst = (i32*)get_left_data(ch);
			i32* p_src = (i32*)src.get_buf_in_sample(srcSample, ch);
			for (i32 s = 0; s < appendSample; s++) {
				*p_dst = *p_src;
				p_dst += stride;
				p_src += stride;
			}
		}
		_channel_infos[ch]._valid_samples += appendSample;
		return true;
#endif
	};
	INLINE b1 append(audio_samples_c& src, i32 srcSample, i32 appendSample) {
		i32 copyByte = appendSample * _info->_bytes_per_sample;
		PLC_MEM_CPY(get_left_data(), src.get_buf_in_sample(srcSample), copyByte);
		_channel_infos[0]._valid_samples += appendSample;
		return true;
	};
	INLINE b1 append(i32 appendSample,i16 ch) {
#if 0
		_valid_samples += appendSample;
		_size += appendSample * _info->_bytes_per_sample;
		return true;
#else
		_channel_infos[ch]._valid_samples += appendSample;
		return true;
#endif
	};

	INLINE b1 append(i32 appendSample) {
		return append(appendSample, 0);
	};
	INLINE b1 append_fully(audio_samples_c& src, i32* usedSamples) {
		i32 AppendSamples;
		AppendSamples = get_samples_max() - get_valid_samples();
		AppendSamples = AppendSamples < src.get_valid_samples() ? AppendSamples : src.get_valid_samples();
		append(src, src.get_used_samples(), AppendSamples);
		*usedSamples = AppendSamples;
		return true;
	};
	INLINE b1 used(i32 usedSample, i16 ch) {
#if 0
		_off += _used_sample * _info->_bytes_per_sample;
		_size -= _used_sample * _info->_bytes_per_sample;
		_valid_samples -= _used_sample;
		_usedSamples += _used_sample;
		return true;
#else
		_channel_infos[ch]._used_sample += usedSample;
		_channel_infos[ch]._valid_samples -= usedSample;
		return true;
#endif
	};
	INLINE b1 used(i32 usedSample) {
		return used(usedSample, 0);
	};
	INLINE b1 clear_used(i16 ch) {
		i32 stride = _info->_channels;
		if (_info->_width == 2) {
			i16* p_dst = (i16*)get_buf_in_sample(0, ch);
			i16* p_src = (i16*)get_buf_in_sample(_channel_infos[ch]._used_sample, ch);
			for (i32 s = 0; s < _channel_infos[ch]._valid_samples; s++) {
				*p_dst = *p_src;
				p_dst += stride;
				p_src += stride;
			}
		}
		else if (_info->_width == 4) {
			i32* p_dst = (i32*)get_buf_in_sample(0, ch);
			i32* p_src = (i32*)get_buf_in_sample(_channel_infos[ch]._used_sample, ch);
			for (i32 s = 0; s < _channel_infos[ch]._valid_samples; s++) {
				*p_dst = *p_src;
				p_dst += stride;
				p_src += stride;
			}
		}
		_channel_infos[ch]._used_sample = 0;
		return true;
	};
	INLINE b1 clear_used() {
		PLC_MEM_MOVE(
			get_buf_in_sample(0),
			get_buf_in_sample(_channel_infos[0]._used_sample), 
			_channel_infos[0]._valid_samples * _info->_bytes_per_sample);
		_channel_infos[0]._used_sample = 0;
		return true;
	};
	INLINE b1 clear(i32 _used_sample, i16 ch) { used(_used_sample, ch); clear_used(ch); return true; };
	INLINE b1 clear(i32 usedSample) { used(usedSample); clear_used(); return true; };
protected:
	channel_info_c _channel_infos[1];
	i16 _fp_num = 0;
	i32 _samples = 0;
	//i32 _valid_samples = 0;
	//i32 _usedSamples = 0;
	i32 _samples_total = 0;//_samples * channel
};






