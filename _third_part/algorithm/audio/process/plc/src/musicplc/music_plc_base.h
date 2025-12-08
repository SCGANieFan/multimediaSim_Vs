#pragma once
#include "gaapi.h"
#include "gaapi_base.h"
#include "gaapi_memory.h"

using namespace plc_gaapi_ns;
using plc_base_port_t = GaapiBasePort_t;
using plc_memory_manger_c = GaapiMemoryInstance_c<8>;

#define PLC_MEM_CPY(dst,src,size)	gaapi_memcpy((void*)(dst),(const void*)(src),(i32)(size))
#define PLC_MEM_SET(dst,val,size)	gaapi_memset((void*)(dst),(u8)(val),(i32)(size))
#define PLC_MEM_MOVE(dst,src,size)  gaapi_memmove((void*)(dst),(const void*)(src),(i32)(size))	
#define PLC_ASSERT(s)				ASSERT(s)


