#pragma once

enum smf_msg_e {
	SMF_MSG_CORE = 0x002,
	SMF_MSG_POOL = 0x020,
	SMF_MSG_HOOK = 0x030,
	SMF_MSG_FS = 0x040,
	SMF_MSG_PROXY = 0x050,
	SMF_MSG_MMGR = 0x060,
	SMF_MSG_BTAPI = 0x068,
	//
	SMF_MSG_SSVC = 0x080,//080~0ff
	//
	SMF_MSG_Device = 0x200,
	//SMF_MSG_POOL = 0x210,
	SMF_MSG_CMD = 0x220,
	//SMF_MSG_FS = 0x230,
	SMF_MSG_VOIP = 0x240,
	SMF_MSG_SHM = 0x250,
	//SMF_MSG_PROXY = 0x260,
	SMF_MSG_FRAME = 0x270,
	SMF_MSG_STREAM = 0x280,
	SMF_MSG_VIDEO = 0x290,
	SMF_MSG_LCDC = 0x2a0,
	SMF_MSG_TWSPB = 0x2b0,
	SMF_MSG_VAD = 0x2c0,
	//SMF_MSG_HOOK = 0x2d0,
	SMF_MSG_MSVC = 0x2e0,
	//
	SMF_MSG_AudioPlayer = 0x100,
	SMF_MSG_AudioRecord = 0X140,
	SMF_MSG_VideoPlayer = 0x180,
	SMF_MSG_VideoRecord = 0X1c0,
	//
	SMF_MSG_LocalPlayer = 0x340,
	SMF_MSG_LocalRecord = 0x350,
	SMF_MSG_LocalSco = 0x360,
	SMF_MSG_APPlayer = 0x370,
	SMF_MSG_APRecord = 0x378,
	//SMF_MSG_APSco = 0x380,
	SMF_MSG_Jpeg = 0x3d0,
	SMF_MSG_ESIMSco = 0x3e0,
	SMF_MSG_LocalKws = 0x3f0,

	SMF_MSG_MAX = 0x400,
};


