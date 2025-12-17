#pragma once

class buffer_c
{
public:
	buffer_c() {};
	buffer_c(u8* buf, i32 max) { _buf = buf; _max = max; };
	//INLINE buffer_c(u8* buf, i32 max) { _buf = buf; _max = max; };
	~buffer_c() {};
public:
	INLINE void Init(u8* buf, i32 max) { _buf = buf; _max = max; };
public:
	u8* _buf = 0;
	i32 _max = 0;
};