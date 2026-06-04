#pragma once
#include <stdint.h>
namespace smf {
	enum EFlags : uint32_t {
		IS_Audio = 1u << 0,
		IS_Video = 1u << 1,
		IS_Stream = 1u << 2,
		IS_Other = 1u << 5,
		IS_Log = 1u << 6,
		IS_IO = 1u << 7,
		IS_List = 1u << 8,
		IS_Filter = 1u << 9,
		IS_Sink = 1u << 10,
		IS_Source = 1u << 11,
		IS_Element = 1u << 12,
		IS_Service = 1u << 13,
		IS_Pipeline = 1u << 14,
		IS_MsgClient = 1u << 15,
		//
		IS_ASync = 1u << 16,
		IS_Destroy = 1u << 17,
		IS_Changed = 1u << 18,
		IS_EOS = 1u << 19,
		IS_Error = 1u << 20,
		IS_Loop = 1u << 21,
		IS_Overlay = 1u << 22,
		IS_Server = 1u << 23,
		IS_Client = 1u << 24,
		IS_Master = 1u << 25,
		IS_Slave = 1u << 26,
		IS_Live = 1u << 27,
		IS_SyncError = 1u << 28,
		IS_StartWithThread = 1u << 30,
		IS_Scheduled = 1u << 31,
	};

	enum ExtFlags : uint32_t {
		EF_Disable = 1u << 31,
		EF_Pause = 1u << 30,
		//
		EF_Debug_0 = 1u << 0x10,
		EF_Debug_1 = 1u << 0x11,
		EF_Debug_2 = 1u << 0x12,
		EF_Debug_3 = 1u << 0x13,
		EF_Debug_4 = 1u << 0x14,
		EF_Debug_5 = 1u << 0x15,
		EF_Debug_6 = 1u << 0x16,
		EF_Debug_7 = 1u << 0x17,
	};
}

#define dbgExtCheck(n)		(_flagsExt.Check(EF_Debug_0<<n))
#define dbgExt(n,...)		if(_flagsExt.Check(EF_Debug_0<<n))dbgChn(ext,##__VA_ARGS__)
#define dbgExtX(n,...)		if(_flagsExt.Check(EF_Debug_0<<n))dbgChnX(ext,##__VA_ARGS__)
#define dbgExtP(n,...)		if(_flagsExt.Check(EF_Debug_0<<n))dbgChnP(ext,##__VA_ARGS__)
#define dbgExtDL(n,v)		if(_flagsExt.Check(EF_Debug_0<<n))dbgChnDL(ext,v)
#define dbgExtSL(n,v)		if(_flagsExt.Check(EF_Debug_0<<n))dbgChnSL(ext,v)
#define dbgExtPL(n)			if(_flagsExt.Check(EF_Debug_0<<n))dbgChnPL(ext)
#define dbgExtPXL(n,s,...)	if(_flagsExt.Check(EF_Debug_0<<n))dbgChnPXL(ext,s,##__VA_ARGS__)
#define dbgExtPDL(n,v)		if(_flagsExt.Check(EF_Debug_0<<n))dbgChnPDL(ext,v)
#define dbgExtPPL(n,v)		if(_flagsExt.Check(EF_Debug_0<<n))dbgChnPPL(ext,v)
#define dbgExtPQL(n,v)		if(_flagsExt.Check(EF_Debug_0<<n))dbgChnPQL(ext,v)
#define dbgExtPSL(n,v)		if(_flagsExt.Check(EF_Debug_0<<n))dbgChnPSL(ext,v)
#define dbgExtDump(n,d,z)	if(_flagsExt.Check(EF_Debug_0<<n))dbgChnDump(ext,d,z)
////////////////////////////////////////////////////////////////////////////