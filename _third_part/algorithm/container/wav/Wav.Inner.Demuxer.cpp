#if 1
#include"Wav.Inner.Demuxer.h"

b1 WavDemuxer_c::Init(AlgoBasePorting_t* basePorting) {
	//check
	if (!basePorting
		|| !basePorting->Malloc
		|| !basePorting->Free)
		return false;

	//
	ALGO_MEM_SET((u8*)this, 0, sizeof(WavDemuxer_c));

	_wavMem.Init(basePorting);

	Buffer buff;
	i32 bufLen = 1024;
	u8* pBuf = (u8*)_wavMem.Malloc(bufLen);
	ALGO_MEM_SET((u8*)pBuf, 0, bufLen);
	buff.Init(pBuf, bufLen);
	_iCache.Init(&buff);

	_stage = WavDemuxStage::WavDemuxStage_SearchRIFF;
	_isDemuxFinish = false;

	return true;
}
void WavDemuxer_c::DeInit() {
	_wavMem.FreeAll();
}

i32 WavDemuxer_c::Receive(u8 * in, i32 inLen) {
	//check
	if (!in)
		return false;

	Buffer buff;

	Data pIn;
	buff.Init(in, inLen);
	pIn.Init(&buff);
	pIn.Append(inLen);

	i32 needByte = 1;
	while (pIn.GetSize() > 0) {
		i32 sizee;
		_iCache.AppendFully(pIn.GetData(), pIn.GetSize(), &sizee);
		pIn.Used(sizee);
		while (_iCache.GetSize() > needByte) {
			switch (_stage)
			{
			case WavDemuxStage::WavDemuxStage_SearchRIFF:
			{
				needByte = 4;
				const i8* riffKey = "RIFF";
				//i32 skipByte = 0;
				i32 inRemByte = inLen;
				while (_iCache.GetSize() > needByte) {
					if (*(u32*)_iCache.GetData() == (*(u32*)riffKey)) {
						_stage = WavDemuxStage::WavDemuxStage_RIFF;
						//_iCache.Used(needByte);
						_iCache.ClearUsed();
						break;
					}
					_iCache.Used(1);
				}
				break;
			}
			case WavDemuxStage::WavDemuxStage_RIFF:
			{
				needByte = sizeof(RiffChunk);
				if (_iCache.GetSize() > needByte)
				{
					_head.riff = *(RiffChunk*)_iCache.GetData();
					_iCache.Used(needByte);
					_stage = WavDemuxStage::WavDemuxStage_Format;
				}
				break;
			}
			case WavDemuxStage::WavDemuxStage_Format:
			{
				needByte = sizeof(FmtChunk);
				if (_iCache.GetSize() > needByte)
				{
					_head.fmt = *(FmtChunk*)_iCache.GetData();
					_iCache.Used(needByte);
					_stage = WavDemuxStage::WavDemuxStage_ExpandFormat;
				}
				break;
			}
			case WavDemuxStage::WavDemuxStage_ExpandFormat:
			{
				needByte = (_head.fmt.chunkSize + 8) - sizeof(FmtChunk);
				if (needByte > 0) {
					if (_iCache.GetSize() > needByte) {
						_head.expandFmt = (ExpandFormatChunk*)_wavMem.Malloc(needByte);
						ALGO_MEM_CPY(_head.expandFmt, _iCache.GetData(), needByte);
						_iCache.Used(needByte);
						_stage = WavDemuxStage::WavDemuxStage_SearchFact;
					}
				}
				else {
					_stage = WavDemuxStage::WavDemuxStage_SearchFact;
				}
				break;
			}
			case WavDemuxStage::WavDemuxStage_SearchFact:
			{
				needByte = 4;
				if (_iCache.GetSize() > needByte) {
					const i8* factKey = "fact";
					if (*(u32*)_iCache.GetData() == *(u32*)factKey) {
						_stage = WavDemuxStage::WavDemuxStage_Fact;
					}
					else {
						_stage = WavDemuxStage::WavDemuxStage_DataHead;
					}
				}
				break;
			}
			case WavDemuxStage::WavDemuxStage_Fact:
			{
				needByte = sizeof(FactChunk);
				if (_iCache.GetSize() > needByte) {
					_head.fact = (FactChunk*)_wavMem.Malloc(sizeof(FactChunk));
					*_head.fact = *(FactChunk*)_iCache.GetData();
					_iCache.Used(needByte);
					_stage = WavDemuxStage::WavDemuxStage_DataHead;
				}
				break;
			}
			case WavDemuxStage::WavDemuxStage_DataHead:
			{
				needByte = sizeof(DataChunk);
				if (_iCache.GetSize() > needByte) {
					_head.data = *(DataChunk*)_iCache.GetData();
					_iCache.Used(needByte);

					_stage = WavDemuxStage::WavDemuxStage_Data;
				}
				break;
			}
			case WavDemuxStage::WavDemuxStage_Data:
			{
				_isDemuxFinish = true;
				return true;
			default:
				break;
			}
			}
		}
	}
	return true;
}


#endif