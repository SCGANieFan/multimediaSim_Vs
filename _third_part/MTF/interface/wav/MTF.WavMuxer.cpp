#include<stdio.h>
#include"MTF.WavMuxer.h"
#include"MTF.Objects.h"
#include"MAF.h"
using namespace mtf_ns;
void mtf_wav_muxer_register()
{
    MTF_Objects::Registe<MTF_WavMuxer>("wav_muxer");
    MAF_REGISTER(wav_mux);
}


MTF_WavMuxer::MTF_WavMuxer()
{

}

MTF_WavMuxer::~MTF_WavMuxer()
{
    fseek((FILE*)_pFile, 0, SEEK_SET);
    MAF_Get(_hd, "headInfo", (void**)_head.Data());
    fwrite(_head.Data(), _head._size, 1, (FILE*)_pFile);
    if (_hd){
        MAF_Deinit(_hd);
        MTF_FREE(_hd);
    }
    if (_pFile)
        fclose((FILE*)_pFile);
}


mtf_i32 MTF_WavMuxer::Init()
{
    MTF_PRINT();
    if (!_url) {
        MTF_PRINT("error, _url = 0");
        return -1;
    }
    _pFile = fopen(_url, "wb+");
    if (!_pFile) {
        MTF_PRINT("error, no such file:%s", _url);
        return -1;
    }
    
#if 1
    const char* type = "wav_mux";
    MA_Ret ret;
    ret = MAF_GetHandleSize(type, &_hdSize);
    if (ret != MA_RET_SUCCESS)
        MTF_PRINT("err");
    if (_hdSize < 1)
        MTF_PRINT("err");
    _hd = MTF_MALLOC(_hdSize);
    if (!_hd)
        MTF_PRINT("err");
#if 1
    mtf_void* param[] = {
    (mtf_void*)type,
    (mtf_void*)Malloc,
    (mtf_void*)Realloc,
    (mtf_void*)Calloc,
    (mtf_void*)Free,
    };

    const char* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4"\
        ";";
    ret = MAF_Init(_hd, script, param);
    if (ret != MA_RET_SUCCESS)
        MTF_PRINT("err");
    mtf_i32 headSize;
    MAF_Get(_hd, "headSize", (void**)&headSize);
    mtf_u8* buf = (mtf_u8*)MTF_MALLOC(headSize);
    _head.Init(buf, headSize);
    _head._size = headSize;
    fseek((FILE*)_pFile, headSize, SEEK_SET);

    MAF_Set(_hd, "rate", (void**)_rate);
    MAF_Set(_hd, "ch", (void**)_ch);
    MAF_Set(_hd, "width", (void**)_width);
    MAF_Set(_hd, "basicInfo", 0);
#endif
#endif
    return 0;
}

mtf_i32 MTF_WavMuxer::receive(MTF_Data& iData)
{
    AA_Data AA_iData;
    MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));

    AA_iData.buff = iData.Data();
    AA_iData.max = AA_iData.size = iData._size;
    MAF_Run(_hd, &AA_iData, 0);

#if 1
    fwrite(iData.Data(), 1, iData._size, (FILE*)_pFile);
    iData.Used(iData._size);
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



