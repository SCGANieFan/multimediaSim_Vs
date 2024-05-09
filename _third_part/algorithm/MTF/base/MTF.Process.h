#pragma once
#include"MTF.Element.h"
class MTF_Process :public MTF_Element
{
public:
	MTF_Process();
	virtual ~MTF_Process();

private:
	virtual mtf_int32 Pull(MTF_Data*& iData) final;
	virtual mtf_int32 Push(MTF_Data& oData) final;
	virtual mtf_int32 Receive(MTF_Data& iData) final;
	virtual mtf_int32 Generate(MTF_Data*& oData) final;
	virtual mtf_int32 Run() final;

protected:
	virtual mtf_int32 Init() = 0;
	virtual mtf_int32 receive(MTF_Data& iData) = 0;
	virtual mtf_int32 generate(MTF_Data*& oData) = 0;
public:
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) override;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) override;

protected:

private:
	MTF_Element* _from;
	MTF_Element* _to;
	//mtf_bool isInit = false;
};

