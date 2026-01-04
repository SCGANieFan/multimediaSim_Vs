#pragma once
#include"MTF.Element.h"
class MTF_Source :public MTF_Element
{
public:
	MTF_Source();
	~MTF_Source();
private:
	virtual mtf_i32 Pull(MTF_Data*& iData)final { return 0; };
	virtual mtf_i32 Push(MTF_Data& oData) final;
	virtual mtf_i32 Receive(MTF_Data& iData)final { return 0; };
	virtual mtf_i32 Generate(MTF_Data*& oData) override;
	virtual mtf_i32 Run() final;
protected:
	virtual mtf_i32 Init() = 0;
	//virtual mtf_i32 DeInit() = 0;
public:
	virtual mtf_i32 generate(MTF_Data*& oData) = 0;
	virtual mtf_i32 Set(const char* key, mtf_void* val) override;
	virtual mtf_i32 Get(const char* key, mtf_void* val) override;
protected:
private:
	MTF_Element* _to;
};

