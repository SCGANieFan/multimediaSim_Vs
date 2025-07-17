#if 1
#include"Wav.Inner.Muxer.h"

#if 1

b1 WavMuxer_c::Init(AlgoBasePorting_t* basePorting) {
	//check
	if (!basePorting
		|| !basePorting->Malloc
		|| !basePorting->Free)
		return false;

	ALGO_MEM_SET(this, 0, sizeof(WavMuxer_c));
	_head.riff.chunkID = *(u32*)"RIFF";
	_head.riff.chunkSize = sizeof(RiffChunk)+ sizeof(FmtChunk) + sizeof(DataChunk) - 8;
	_head.riff.format = *(u32*)"WAVE";

	_head.fmt.chunkId = *(u32*)"fmt ";//
	_head.fmt.chunkSize = 16;
	_head.fmt.audioFormat = 1;
#if 0
	_head.fmt.numChannels = paramIn->channels;
	_head.fmt.sampleRate = paramIn->fsHz;
	_head.fmt.byteRate = paramIn->channels * paramIn->fsHz * width;
	_head.fmt.blockAlign = paramIn->channels * width;
	_head.fmt.bitsPerSample = width << 3;
#endif
	_head.data.chunkId = *(u32*)"data";
	//_head.data.chunkSize

	_wavMem.Init(basePorting);
	return true;
}


b1 WavMuxer_c::Process(u8* inOut, i32 inLen) {
	//check
	if (!inOut
		|| inLen < 0)
		return false;
#if 0
	_head.data.chunkSize += inLen;
	_head.riff.chunkSize = _head.data.chunkSize + sizeof(WavHead) - 8;
#else
	_head.data.chunkSize += inLen;
	_head.riff.chunkSize += inLen;
#endif
	if (_head.fmt.bitsPerSample == 8){
		for (i32 i = 0; i < inLen; i++){
			inOut[i] += 0x80;
		}
	}
	return true;
}

#endif

#endif