#pragma once
#include"MTF.Element.h"
class MTF_Process :public MTF_Element
{
public:
	MTF_Process();
	virtual ~MTF_Process();

private:
	virtual mtf_i32 Pull(MTF_Data*& iData) final;
	virtual mtf_i32 Push(MTF_Data& oData) final;
	virtual mtf_i32 Receive(MTF_Data& iData) final;
	virtual mtf_i32 Generate(MTF_Data*& oData) final;
	virtual mtf_i32 Run() final;

protected:
	virtual mtf_i32 Init() = 0;
	virtual mtf_i32 receive(MTF_Data& iData) = 0;
	virtual mtf_i32 generate(MTF_Data*& oData) = 0;
public:
	virtual mtf_i32 Set(const char* key, mtf_void* val) override;
	virtual mtf_i32 Get(const char* key, mtf_void* val) override;

protected:

private:
	MTF_Element* _from;
	MTF_Element* _to;
	//mtf_bool isInit = false;
};

