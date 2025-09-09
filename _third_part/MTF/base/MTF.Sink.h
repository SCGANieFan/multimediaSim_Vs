#pragma once
#include"MTF.Element.h"
class MTF_Sink :public MTF_Element
{
public:
	MTF_Sink();
	~MTF_Sink();
private:
	virtual mtf_i32 Pull(MTF_Data*& iData) final;
	virtual mtf_i32 Push(MTF_Data& oData)final { return 0; };
	virtual mtf_i32 Receive(MTF_Data& iData) override;
	virtual mtf_i32 Generate(MTF_Data*& oData)final { return 0; };
	virtual mtf_i32 Run() final;
protected:
	virtual mtf_i32 Init() = 0;
public:
	virtual mtf_i32 receive(MTF_Data& iData) = 0;
	virtual mtf_i32 Set(const char* key, mtf_void* val) override;
	virtual mtf_i32 Get(const char* key, mtf_void* val) override;

private:
	MTF_Element* _from;

};

