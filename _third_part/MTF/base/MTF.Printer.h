#pragma once
#include"MTF.Type.h"
#include"MTF.String.h"

class MTF_Printer
{
public:
	MTF_Printer();
	~MTF_Printer();
public:
	static mtf_void Printf(const char* _Format, ...);
private:
};
#define MTF_PRINTORI(fmt,...)	MTF_Printer::Printf(fmt, ##__VA_ARGS__)
#define MTF_PRINT(fmt,...)		MTF_Printer::Printf("<%s>[%s](%d)" fmt "\n", MTF_String::BaseName(__FILE__), __func__, __LINE__, ##__VA_ARGS__)
