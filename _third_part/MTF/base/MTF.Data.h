#pragma once
#include"MTF.Type.h"
enum MTF_DataFlag {
	MTF_DataFlag_ESO=1<<0,
	MTF_DataFlag_EMPTY=1<<1,
	MTF_DataFlag_EXTRA_INFO=1<<2,
	MTF_DataFlag_INCOMPLETE=1<<3,
};

class MTF_Data
{
public:
	MTF_Data();
	~MTF_Data();

public:
	mtf_i32 Init(mtf_u8* buff, mtf_i32 len);
	mtf_i32 DeInit();
	mtf_i32 Append(mtf_u8* buff, mtf_i32 len);
	mtf_void Clear();
	mtf_void Clear(mtf_i32 len);
	mtf_u8* Data();
	mtf_u8* LeftData();
	mtf_i32 LeftSize();
	mtf_i32 Used(mtf_i32 size);
	mtf_u8* Buff();
	mtf_i32 Max() { return _max; }
public:
	mtf_i32 _size;
	mtf_i32 _flags;
private:
	mtf_u8* _buff;
	mtf_i32 _off;
	mtf_i32 _max;
};

