#include"MTF.WavMuxer.h"
#include"MTF.Objects.h"
#include"MTF.Porting.h"
#include "WavMux.h"
using namespace mtf_ns;
void mtf_wav_muxer_register()
{
    MTF_Objects::Registe<MTF_WavMuxer>("wav_muxer");
}


MTF_WavMuxer::MTF_WavMuxer()
{

}

MTF_WavMuxer::~MTF_WavMuxer()
{
    
    FileSeekPorting(_pFile, 0, FileSeekPorting_e::FILE_PORTING_SEEK_SET);
    if(_hd)
        WavMux_Get(_hd, WAV_MUX_GET_CHOOSE_HEAD, (void**)_head.Data());
    FileWritePorting(_pFile, _head.Data(), _head._size);
    if (_hd){
        MTF_PRINT();
#if 1
        WavMux_DeInit(_hd);
        Free(_hd);
        Free(_basePorting);
#endif
    }
    if (_pFile)
        FileClosePorting(_pFile);
}

static mtf_void* MallocLocal(int32_t size)
{
#if 1
    static mtf_i32 sizeTotal = 0;
    sizeTotal += size;
    mtf_void* ptr = Malloc(size);
    MTF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (mtf_u32)ptr, size, sizeTotal);
    return ptr;
#else
    return Malloc(size);
#endif	
}

static mtf_void FreeLocal(mtf_void* block)
{
#if 1
    MTF_PRINT("free, ptr:%x", (mtf_u32)block);
#endif
    return Free(block);
}


mtf_i32 MTF_WavMuxer::Init()
{
    MTF_PRINT();
    if (!_url) {
        MTF_PRINT("error, _url = 0");
        return -1;
    }
    _pFile = FileOpenPorting(_url, "wb+");
    if (!_pFile) {
        MTF_PRINT("error, no such file:%s", _url);
        return -1;
    }
    
#if 1
    WavMuxInitParam initParam;
    MTF_MEM_SET(&initParam, 0, sizeof(WavMuxInitParam));
    _basePorting = Malloc(sizeof(AlgoBasePorting_t));
    AlgoBasePorting_t* basePorting = (AlgoBasePorting_t*)_basePorting;

    basePorting->Malloc = MallocLocal;
    basePorting->Free = FreeLocal;
    initParam.basePorting = basePorting;
    _hdSize = WavMux_GetSize();
    _hd = Malloc(_hdSize);
    MTF_PRINT("_hd=%x,size:%d", (mtf_u32)_hd, _hdSize);
    if (!_hd)
    {
        return -1;
    }

    mtf_i32 ret = WavMux_Init(_hd, &initParam);

    if (ret < 0)
    {
        return -1;
    }
#endif
    mtf_i32 headSize;
    WavMux_Get(_hd, WAV_MUX_GET_CHOOSE_HEAD_SIZE, (void**)&headSize);
    mtf_u8* buf = (mtf_u8*)MTF_MALLOC(headSize);
    _head.Init(buf, headSize);
    _head._size = headSize;
    FileSeekPorting(_pFile, headSize, FileSeekPorting_e::FILE_PORTING_SEEK_SET);
    void* param[] = { (void*)_rate,(void*)_ch,(void*)_width };
    WavMux_Set(_hd, WAV_MUX_SET_CHOOSE_BASIC_INFO, param);
    return 0;
}

mtf_i32 MTF_WavMuxer::receive(MTF_Data& iData)
{
    mtf_i32 ret;
    ret = WavMux_Run(_hd,
        iData.Data(),
        iData._size,
        0, 0);

    if (ret < 0)
    {
        return -1;
    }
#if 1
    FileWritePorting(_pFile, iData.Data(), iData._size);
    iData.Used(iData._size);
    iData.Clear();
    if (iData._flags & MTF_DataFlag_ESO)
        return -1;
#endif
    return 0;
}

mtf_i32 MTF_WavMuxer::Set(const char* key, mtf_void* val)
{
    if (MTF_String::StrCompare(key, "url"))
    {
        MTF_PRINT("url,%s", (const char*)val);
        _url = (const char*)val;
        return 0;
    }
    return MTF_Sink::Set(key, val);
}
mtf_i32 MTF_WavMuxer::Get(const char* key, mtf_void* val)
{
    return MTF_Sink::Get(key, val);
}



