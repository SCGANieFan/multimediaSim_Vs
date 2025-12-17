#pragma once

#include "music_plc_data.h"



class audio_info_c
{
public:
	audio_info_c() {};
	~audio_info_c() {};
public:
	INLINE b1 init(i32 rate, i16 width, i16 channels) {
		_rate = rate;
		_width = width;
		_channels = channels;
		_bytes_per_sample = _width * _channels;
		return true;
	};
public:
	i32 _rate = 0;
	i16 _width = 0;
	i16 _channels = 0;
	i32 _bytes_per_sample = 0;
};

class audio_data_c
{
public:
	audio_data_c() {};
	~audio_data_c() {};
public:
	INLINE b1 init(const audio_info_c* p_info, buffer_c* buffer) {
		_buff = buffer->_buf;
		_max = buffer->_max;
		_info = p_info;
		return true;
	}
public:
	u8* _buff = 0;
	i32 _max = 0;
	const audio_info_c* _info = 0;
};




