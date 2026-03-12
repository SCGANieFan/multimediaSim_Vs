
#include"MTF.Objects.h"
#include"MTF.Element.h"
#include"MTF.String.h"

#include"MTF.Memory.h"
#include"MTF.h"

using namespace mtf_ns;


namespace MTFApi_ns {
bool PraseElement(const char* str, int32_t strLen, MTF_Element**ele, void** param)
{

	const char* pStr = str;
	int32_t strLen0 = strLen;

	if (strLen0 < 3)
		return false;

	int32_t posStart;
	int32_t posEnd;

	posStart = MTF_String::Search(pStr, "|", 0, strLen0 - 1);
	if (posStart < 0)
		return false;
	*ele = (MTF_Element*)MTF_Objects::Create(pStr + posStart + 1);
	pStr += posStart + 1;
	strLen0 -= posStart + 1;

	while (1)
	{
		posStart = MTF_String::Search(pStr, ",", 0, strLen0 - 1);
		if (posStart < 0)
		{
			if((*ele)->Init())
				return false;
			return true;
		}

		pStr += posStart + 1;
		strLen0 -= posStart + 1;
		posStart = MTF_String::Search(pStr, "=", 0, strLen0 - 1);
		uint8_t shift = 1;
		if (*(pStr + posStart + 1) == '$')
		{
			uint8_t index;
			index = *(pStr + posStart + 2) - 48;
			shift = 3;
			if (*(pStr + posStart + 3) != ','
				&& *(pStr + posStart + 3) != ';'
				&& *(pStr + posStart + 3) != '|')
			{
				index = 10 * index + *(pStr + posStart + 3) - 48;
				shift = 4;
			}
			(*ele)->Set(pStr, param[index]);
		}
		pStr += posStart + shift;
		strLen0 -= posStart + shift;
	}
}


void* MTf_Malloc_cb(int32_t size){
	return malloc(size);
}
void* MTf_Realloc_cb(void* block, int32_t size){
	return realloc(block, size);
}
void* MTf_Calloc_cb(int32_t count, int32_t size) {
	return calloc(count, size);
}
void MTf_Free_cb(void* block) {
	free(block);
}

#define GBL_BYTE 100*1024
static mtf_u8 gbl_heap[GBL_BYTE];

int32_t MTFApi::Init()
{
	MTF_PRINT();
	//MTF_PRINT_ERR();
	//MTF_PRINT_WARN();
	//MTF_PRINT_NOTE();

	//MTF_MemoryRegister("gbl", MTf_Malloc_cb, MTf_Realloc_cb, MTf_Calloc_cb, MTf_Free_cb);
	MTF_MemoryRegister("gbl", gbl_heap, sizeof(gbl_heap));

#if 0
	MTF_REGISTER(pcm_demuxer);
	MTF_REGISTER(music_plc);
	MTF_REGISTER(pcm_muxer);
#endif
#if 0
	constexpr mtf_u32 id32_0 = STR2ID32L("12342");
	constexpr mtf_u32 id32_1 = STR2ID32L("22341");
	mtf_u32 tmp = 0xc0f8bfa;
	if (tmp == STR2ID32L("12342")) {
		MTF_PRINT("equal");
	}
	else{
		MTF_PRINT("not equal");
	}
	MTF_PRINT("%0x", id32_0);
	MTF_PRINT("%0x", id32_1);

	constexpr mtf_u64 id = STR2ID64NL("123");
	char s[8];
	ID2STRNL(id, s, sizeof(s));
	MTF_PRINT("%s", s);
#endif
#if 1
	MTF_Object *obj=new MTF_Object();
	delete obj;
#endif
	return 0;
}

MTF_Element* eles[10];

int32_t MTFApi::Api(const char* str, void** param)
{
	const char* pStr = str;
	int32_t strLen = MTF_String::StrLen(pStr);
	int32_t posStart;
	int32_t posEnd;

	int32_t i = 0;
	while (1)
	{
		posStart = MTF_String::Search(pStr, "|", 0, strLen - 1);
		if (posStart < 0)
			break;
		posEnd = MTF_String::Search(pStr, "|", posStart + 1, strLen - 1);
		bool ret = PraseElement(pStr + posStart, posEnd - posStart + 1, &eles[i], param);
		if (!ret) {
			return -1;
		}
		if (i > 0)
		{
			eles[i - 1]->Set("to", eles[i]);
			eles[i]->Set("from", eles[i-1]);
		}
		pStr += posEnd + 1;
		strLen -= posEnd + 1;
		i++;
	}
	while(eles[0]->Run()==0);

	for (int32_t i = 0; i < sizeof(eles) / sizeof(eles[0]);i++)
	{
		if (eles[i])
			eles[i]->~MTF_Element();
	}
	return 0;
}


}