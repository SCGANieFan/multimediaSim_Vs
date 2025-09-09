#include "MTF.AudioMuxer.h"
#include "MTF.String.h"

MTF_AudioMuxer::MTF_AudioMuxer()
{
}
MTF_AudioMuxer::~MTF_AudioMuxer()
{
}



mtf_i32 MTF_AudioMuxer::receive(MTF_Data& iData)
{
	return 0;
}

mtf_i32 MTF_AudioMuxer::Set(const char* key, mtf_void* val)
{
	return MTF_Sink::Set(key, val) & MTF_AudioInfo::Set(key, val);
}
mtf_i32 MTF_AudioMuxer::Get(const char* key, mtf_void* val)
{
	return MTF_Sink::Get(key, val) & MTF_AudioInfo::Get(key, val);
}



