#pragma once
#include"MTF.Object.h"
#include"MTF.Data.h"
#if 0
enum MTF_Ret
{
	
};
#endif
class MTF_Element :public MTF_Object
{
public:
	MTF_Element();
	virtual ~MTF_Element();
public:
	virtual mtf_i32 Init() = 0;
	virtual mtf_i32 Run() = 0;

//protected:
	virtual mtf_i32 Pull(MTF_Data*& iData) = 0;
	virtual mtf_i32 Push(MTF_Data& oData) = 0;
	virtual mtf_i32 Receive(MTF_Data& iData) = 0;
	virtual mtf_i32 Generate(MTF_Data*& oData) = 0;

public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) override;
	virtual mtf_i32 Get(const char* key, mtf_void* val) override;

private:

};

