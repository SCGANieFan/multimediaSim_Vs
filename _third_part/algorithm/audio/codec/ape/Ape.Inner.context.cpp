#include"Ape.Inner.context.h"

#if 1

/**************************************************************************************************
WAV header structure
**************************************************************************************************/
typedef struct
{
    // RIFF header
    i8 cRIFFHeader[4];
    u32 nRIFFBytes;

    // data type
    i8 cDataTypeID[4];

    // wave format
    i8 cFormatHeader[4];
    u32 nFormatBytes;

    u16 nFormatTag;
    u16 nChannels;
    u32 nSamplesPerSec;
    u32 nAvgBytesPerSec;
    u16 nBlockAlign;
    u16 nBitsPerSample;

    // data chunk header
    i8 cDataHeader[4];
    u32 nDataBytes;
}WaveHeader;


typedef struct {
    u8 cID[4];                         // should equal 'MAC '
    u16 nVersion;                        // version number * 1000 (3.81 = 3810)
    u16 nCompressionLevel;               // the compression level
    u16 nFormatFlags;                    // any format flags (for future use)
    u16 nChannels;                       // the number of channels (1 or 2)
    u32 nSampleRate;                     // the sample rate (typically 44100)
    u32 nHeaderBytes;                    // the bytes after the MAC header that compose the WAV header
    u32 nTerminatingBytes;               // the bytes after that raw data (for extended info)
    u32 nTotalFrames;                    // the number of frames in the file
    u32 nFinalFrameBlocks;               // the number of samples in the final frame
}ApeHeaderOld;

//inner
STATIC INLINE i32 GetHeaderSize()
{
    i32 size1 = sizeof(ApeDescriptor);
    i32 size2 = sizeof(ApeHeaderOld);
    return size1 > size2 ? size1 : size2;
}


//i32 ApeContext::Parser(u8* in, i32 inByte)
i32 ApeContext::Parser(Data* data)
{
    // ALGO_PRINT();
    u8* pIn = data->GetData();
    i32 inByteMax = data->GetSize();
    i32 inByteUsed = 0;
    ApeContext_t* context = &_context;
    memset(context, 0, sizeof(ApeContext_t));
    //search Head
    // while ((*(u32*)(&pIn[inByteUsed]) != *(u32*)"MAC "))
    while ( pIn[inByteUsed+0] != 'M'
            || pIn[inByteUsed+1] != 'A'
            || pIn[inByteUsed+2] != 'C'
            || pIn[inByteUsed+3] != ' ')
    {
        if (inByteUsed > (inByteMax - 4)) {
            data->Used(inByteUsed);
            return APERET_FAIL;
        }
        inByteUsed++;
    }

    //inbyte 
    if ((inByteMax - inByteUsed) < GetContextSize()) {
        data->Used(inByteUsed);
        return APERET_FAIL;
    }

    ApeDescriptor* descriptor = &context->descriptor;
    ApeHeader* header = &context->header;

    inByteUsed += 4;
    descriptor->fileversion = *((u16*)(&pIn[inByteUsed]));
    inByteUsed -= 4;

    if (descriptor->fileversion >= 3980) {
        ALGO_MEM_CPY(descriptor, &pIn[inByteUsed], sizeof(ApeDescriptor));
        if (descriptor->descriptorlength > sizeof(ApeDescriptor))
            inByteUsed += descriptor->descriptorlength;
        else
            inByteUsed += sizeof(ApeDescriptor);

        ALGO_MEM_CPY(header, &pIn[inByteUsed], sizeof(ApeHeader));
        if (descriptor->headerlength > sizeof(ApeHeader))
            inByteUsed += descriptor->headerlength;
        else
            inByteUsed += sizeof(ApeHeader);

        //seektable
        seektablePos = inByteUsed;
        inByteUsed += descriptor->seektablelength;
        waveHeaderPos = inByteUsed;
        inByteUsed += descriptor->wavheaderlength;
    }
    else {
        //headerOld
        ApeHeaderOld headerOld;
        ALGO_MEM_CPY(&headerOld, pIn, sizeof(ApeHeaderOld));
        //pIn += sizeof(ApeHeaderOld);
        inByteUsed += sizeof(ApeHeaderOld);
        // fail on 0 length APE files (catches non-finalized APE files)
        if (headerOld.nTotalFrames == 0)
            return APE_RET_INVALID_INPUT_FILE;

        //nPeakLevel
#if 0
        i32 nPeakLevel = -1;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_PEAK_LEVEL)
        {
            nPeakLevel = *((i32*)pIn);
            inByteUsed += 4;
        }
#else
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_PEAK_LEVEL)
        {
            inByteUsed += 4;
        }
#endif
        //descriptor
        ALGO_MEM_CPY(descriptor->magic, headerOld.cID, 4);
        descriptor->fileversion = (i32)headerOld.nVersion;
        descriptor->padding = 0;
        descriptor->descriptorlength = 0;
        descriptor->headerlength = 0;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_SEEK_ELEMENTS)
        {
            descriptor->seektablelength = (*((i32*)&pIn[inByteUsed])) * 4;
            inByteUsed += 4;
        }
        else
            descriptor->seektablelength = (i32)(headerOld.nTotalFrames * 4);
        descriptor->wavheaderlength = (headerOld.nFormatFlags & APE_FORMAT_FLAG_CREATE_WAV_HEADER) ? (int64_t)(sizeof(WaveHeader)) : headerOld.nHeaderBytes;
        descriptor->audiodatalength = (uint32_t)0;
        descriptor->audiodatalength_high = (uint32_t)0;
        descriptor->wavtaillength = (uint32_t)0;
        ALGO_MEM_SET(descriptor->md5, 0, sizeof(descriptor->md5));
        //header
        header->compressiontype = (i32)headerOld.nCompressionLevel;
        header->formatflags = (i32)headerOld.nFormatFlags;
        header->blocksperframe = (uint32_t)(((headerOld.nVersion >= 3900) || ((headerOld.nVersion >= 3800) && (headerOld.nCompressionLevel == APE_COMPRESSION_LEVEL_EXTRA_HIGH))) ? 73728 : 9216);
        if (headerOld.nVersion >= 3950)
            header->blocksperframe = 73728 * 4;
        header->finalframeblocks = (uint32_t)headerOld.nFinalFrameBlocks;
        header->totalframes = (uint32_t)headerOld.nTotalFrames;
        header->bps = (headerOld.nFormatFlags & APE_FORMAT_FLAG_8_BIT) ? 8 : ((headerOld.nFormatFlags & APE_FORMAT_FLAG_24_BIT) ? 24 : 16);
        header->channels = (i32)headerOld.nChannels;
        header->samplerate = (i32)headerOld.nSampleRate;

        // check for an invalid blocks per frame
        if (header->blocksperframe > (10 * ONE_MILLION) || header->blocksperframe <= 0)
            return APE_RET_INVALID_INPUT_FILE;

        // check the final frame size being nonsense
        if (header->finalframeblocks > header->blocksperframe)
            return APE_RET_INVALID_INPUT_FILE;

        // get the wave header
        if (!(headerOld.nFormatFlags & APE_FORMAT_FLAG_CREATE_WAV_HEADER) && (headerOld.nHeaderBytes > 0))
        {
            if (headerOld.nHeaderBytes > 1024 * 1024)
                return APE_RET_INVALID_INPUT_FILE;
            waveHeaderPos = inByteUsed;
            inByteUsed += headerOld.nHeaderBytes;
        }

        // get the seek tables (really no reason to get the whole thing if there's extra)
        seektablePos = inByteUsed;
        inByteUsed += descriptor->seektablelength;
    }
    return APERET_SUCCESS;
}
#endif

