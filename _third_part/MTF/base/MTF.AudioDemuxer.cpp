#include "MTF.AudioDemuxer.h"
#include "MTF.String.h"

MTF_AudioDemuxer::MTF_AudioDemuxer()
{
}
MTF_AudioDemuxer::~MTF_AudioDemuxer()
{
}

mtf_i32 MTF_AudioDemuxer::generate(MTF_Data*& oData)
{
	return 0;
}

mtf_i32 MTF_AudioDemuxer::Set(const char* key, mtf_void* val)
{
	return MTF_Source::Set(key, val) & MTF_AudioInfo::Set(key, val);
}
mtf_i32 MTF_AudioDemuxer::Get(const char* key, mtf_void* val)
{
	return MTF_Source::Get(key, val) & MTF_AudioInfo::Get(key, val);
}




