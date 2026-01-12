#include"Ape.Inner.basic.h"
#include"Ape.Inner.context.h"


class ApeDemuxer_c
{
public:
    ApeDemuxer_c() {};
    ~ApeDemuxer_c() {};
    i32 Init()
    {
        context.Init();
        seekTable.Init();
        Buffer buff = { inTempBuff,inTempBuffSize };
        inTemp.Init(&buff);
        _isDemuxContextInitSuccess = false;
        _isDemuxRunFinish = false;
        ALGO_MEM_SET(inTempBuff, 0, inTempBuffSize);
        return APERET_SUCCESS;
    }

    i32 Run(uint8_t* in, int32_t inLen)
    {
        if (_isDemuxRunFinish) {
            return APERET_SUCCESS;
        }

        inTemp.AppendFully(in, inLen, 0);
        if (_isDemuxContextInitSuccess == false) {
            if (inTemp.GetSize() < sizeof(ApeContext_t)) {
                return APERET_SUCCESS;
            }
            if (context.Parser(inTemp.GetData(), inTemp.GetSize()) == APERET_SUCCESS) {
                _isDemuxContextInitSuccess = true;
            }
        }
        
        if (inTemp.GetSize() < context.seektablePos) {
            return APERET_SUCCESS;
        }
        i32* pdata = (i32*)&(inTemp.GetData()[context.seektablePos]);
        seekTable.SetFirstFramePos(*pdata);
        _isDemuxRunFinish = true;
        return APERET_FAIL;
    }

    i32 DeInit()
    {
        ALGO_PRINT();
        return APERET_SUCCESS;
    }

private:
    const STATIC i32 inTempBuffSize = sizeof(ApeContext_t)+512;
public:
    ApeContext context;
    ApeSeekTable_c seekTable;
    Data inTemp;
    b1 _isDemuxContextInitSuccess;
    b1 _isDemuxRunFinish;
    u8 inTempBuff[inTempBuffSize];
};
