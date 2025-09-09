#include "MTF.VideoProcess.h"
#include "MTF.String.h"


MTF_VideoProcess::MTF_VideoProcess()
{

}
MTF_VideoProcess::~MTF_VideoProcess()
{

}


mtf_i32 MTF_VideoProcess::Set(const char* key, mtf_void* val)
{
	return MTF_Process::Set(key, val) & MTF_VideoInfo::Set(key, val);
}

mtf_i32 MTF_VideoProcess::Get(const char* key, mtf_void* val)
{
	return MTF_Process::Get(key, val) & MTF_VideoInfo::Get(key, val);
}
