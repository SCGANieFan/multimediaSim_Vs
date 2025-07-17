#if 1
#include"Algo.Data.h"
#include"Wav.Inner.HeadType.h"
#include"Wav.Inner.Com.h"

#if 1
class WavDemuxer_c
{
private:
	enum class WavDemuxStage {
		WavDemuxStage_SearchRIFF = 0,
		WavDemuxStage_RIFF,
		WavDemuxStage_Format,
		WavDemuxStage_ExpandFormat,
		WavDemuxStage_SearchFact,
		WavDemuxStage_Fact,
		WavDemuxStage_DataHead,
		WavDemuxStage_Data,
	};

public:
	WavDemuxer_c() {};
	~WavDemuxer_c() {};

public:
	INLINE b1 GetIsDemuxFinish() { return _isDemuxFinish; }
	INLINE u32 GetSampleRate() { return _head.fmt.sampleRate; }
	INLINE u16 GetNumChannels() { return _head.fmt.numChannels; }
	INLINE u32 GetWidth() { return _head.fmt.blockAlign / _head.fmt.numChannels; }
	INLINE u32 GetDataPos() { return _head.riff.chunkSize - _head.data.chunkSize + 8; }

public:
	b1 Init(AlgoBasePorting_t* basePorting);
	void DeInit();
	i32 Receive(u8* in, i32 inLen);
private:
	WavHead _head;
	WavMem_c _wavMem;
	Data _iCache;
	WavDemuxStage _stage;
	b1 _isDemuxFinish;
};

#endif

#endif