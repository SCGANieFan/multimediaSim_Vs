#include "MAF.Objects.h"
#include "MAF.String.h"
#include "MAF.Algorithm.h"
#include "MAF.h"
#include<stdlib.h>

static maf_bool prase(maf_void* hd, const maf_int8* script, maf_void** param)
{
	const maf_int8* pStr = script;
	maf_int32 strLen = MAF_String::StrLen(pStr);
	maf_int32 strLen0 = strLen;

	if (strLen0 < 1)
		return false;

	maf_int32 posStart;
	maf_int32 posEnd;
	//type	
	posStart = MAF_String::Search(pStr, "type=", 0, strLen0 - 1);
	if (posStart < 0)
		return false;
	if (*(pStr + posStart + 5) == '$')
		MAF_Object::Create((const maf_int8*)param[*(pStr + posStart + 6) - 48], hd);
	pStr += posStart + 7;
	strLen0 -= posStart + 7;

	//param
	while (1)
	{
		posStart = MAF_String::Search(pStr, ",", 0, strLen0 - 1);
		if (posStart < 0)
		{
			if (((MAF_Algorithm*)hd)->Init())
				return false;
			return true;
		}

		pStr += posStart + 1;
		strLen0 -= posStart + 1;
		posStart = MAF_String::Search(pStr, "=", 0, strLen0 - 1);

		uint8_t shift = 1;
		if (*(pStr + posStart + 1) == '$')
		{
			uint8_t index;
			index = *(pStr + posStart + 2) - 48;
			shift = 3;
			if (*(pStr + posStart + 3) != ','
				&& *(pStr + posStart + 3) != ';')
			{
				index = 10* index +*(pStr + posStart + 3) - 48;
				shift = 4;
			}
			((MAF_Algorithm*)hd)->Set(pStr, param[index]);
		}
		pStr += posStart + shift;
		strLen0 -= posStart + shift;
	}
#if 0
	const char* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4"\
		",rate=$5,ch=$6,width=$7,fSamples=$8,decayMs=$9,overlapMs=$10";
#endif
}

EXTERNC
{
	MA_Ret MAF_GetHandleSize(const char* type, int32_t * size)
	{
	//check
	if (!type
		|| !size)
		return MA_RET_INPUT_ERROR;
	maf_int32 ssize = MAF_Object::GetSize(type);
	if (ssize <= 0)
		return MA_RET_FAIL;
	*size = ssize;
	return MA_RET_SUCCESS;
}



MA_Ret MAF_Init(void* hd, const char* script, void** param)
{
	//check
	if (!hd
		|| !script
		|| !param)
		return MA_RET_INPUT_ERROR;

	maf_bool ret = prase(hd, script, param);
	if(!ret)
		return MA_RET_FAIL;
	return MA_RET_SUCCESS;
}
MA_Ret MAF_Deinit(void* hd)
{
	((MAF_Algorithm*)hd)->Deinit();
	((MAF_Algorithm*)hd)->~MAF_Algorithm();
	return MA_RET_SUCCESS;
}


MA_Ret MAF_Set(void* hd, const char* key,void** val)
{
#if 0
	((MAF_Algorithm*)hd)->Set(key, (void*)val);
	return MA_RET_SUCCESS;
#else
	return ((MAF_Algorithm*)hd)->Set(key, (void*)val);
#endif
}

MA_Ret MAF_Get(void* hd, const char* key,void** val)
{
#if 0
	((MAF_Algorithm*)hd)->Get(key, (void*)val);
	return MA_RET_SUCCESS;
#else
	return ((MAF_Algorithm*)hd)->Get(key, (void*)val);
#endif
}

MA_Ret MAF_Run(void* hd, AA_Data * dataIn, AA_Data * dataOut)
{
	MAF_Data iData;
	MAF_MEM_SET(&iData, 0, sizeof(MAF_Data));
	if (dataIn){
		iData.Init(dataIn->buff + dataIn->off, dataIn->size);
		iData.SetFlag(dataIn->flags);
	}
		
	MAF_Data oData;
	MAF_MEM_SET(&oData, 0, sizeof(MAF_Data));
	if (dataOut){
		oData.Init(dataOut->buff + dataOut->off + dataOut->size, 0, dataOut->max - dataOut->off - dataOut->size);
	}

	((MAF_Algorithm*)hd)->Process(&iData, &oData);

	if (dataIn) {
		int32_t used = dataIn->size - iData.GetSize();
		dataIn->off += used;
		dataIn->size -= used;
		dataIn->flags = iData.GetFlags();
	}
	if (dataOut) {
		dataOut->size += oData.GetSize();
		dataOut->flags = oData.GetFlags();
	}
	return MA_RET_SUCCESS;
}

MA_Ret MAF_Receive(void* hd, AA_Data* dataIn){
	MAF_Data iData;
	MAF_MEM_SET(&iData, 0, sizeof(MAF_Data));
	if (dataIn){
		iData.Init(dataIn->buff + dataIn->off, dataIn->size);
		iData.SetFlag(dataIn->flags);
	}
	((MAF_Algorithm*)hd)->Rceive(&iData);
	if (dataIn) {
		int32_t used = dataIn->size - iData.GetSize();
		dataIn->off += used;
		dataIn->size -= used;
		dataIn->flags = iData.GetFlags();
	}
	return MA_RET_SUCCESS;
}
MA_Ret MAF_Generate(void* hd, AA_Data* dataOut){

	MAF_Data oData;
	MAF_MEM_SET(&oData, 0, sizeof(MAF_Data));
	if (dataOut){
		oData.Init(dataOut->buff + dataOut->off + dataOut->size, 0, dataOut->max - dataOut->off - dataOut->size);
	}

	((MAF_Algorithm*)hd)->Generate(&oData);

	if (dataOut) {
		dataOut->size += oData.GetSize();
		dataOut->flags = oData.GetFlags();
	}
	return MA_RET_SUCCESS;
}

}
