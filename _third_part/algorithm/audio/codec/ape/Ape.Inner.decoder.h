#include"Ape.Inner.basic.h"
#include"Ape.Inner.context.h"
#include"Ape.Inner.rangeCoder.h"
#include"Ape.Inner.NNfilter.h"
#include"Ape.Inner.prediction.h"

class ApeDecoder_c
{
public:
    ApeDecoder_c() {};
    ~ApeDecoder_c() { DeInitInner(); }
public:
    MemoryManger_c* GetMM() { return &MM; }

public:
    i32 Init(AlgoBasePorting_c* basePorting, ApeContext_t* contextIn, u32 startFrameNum, u32 skip);
    i32 ReInit(u32 startFrameNum, u32 skip);
    i32 Run(uint8_t* in, int32_t* inLen, uint8_t* out, int32_t* outLen);
    i32 DeInitInner();

    //private:
public:
    MemoryManger_c MM;
    ApeContext context;

    //rang decode
    ApeRangecoder rc;                                               ///< rangecoder used to decode actual values
    ApeRice riceX;                                                  ///< rice code parameters for the second channel
    ApeRice riceY;                                                  ///< rice code parameters for the first channel

    //NN Filter
    ApeNNFilters NNFilter;

    //preditor
    ApePrediction prediction;

    //decoded pcm buffer
    i32 ch0[APE_BLOCKS_MAX];                                   ///< decoded data for the first channel
    i32 ch1[APE_BLOCKS_MAX];                                   ///< decoded data for the second channel

    //input buffer read
    BufferRead bufferRead;
    ApeFrameInfo frame;

    //other
    i32 fSet;                                                   ///< which filter set to use (calculated from compression level)
    u32 CRC;                                                   ///< frame CRC
    u32 frameFlags;                                            ///< frame flags
    i32 blocksMax;
    u16 blocksDecoded;
    i32 currentframe;

    i32 blocksUsed;
    Data inCache;
    b1 isFrameStart;
    b1 haveInCache;
    b1 isNewFrameStart;
    u32 firstSkip;
};