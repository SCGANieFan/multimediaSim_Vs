#pragma once
#include "Element.h"
#include "smf_pack.h"
#include <stdint.h>
namespace smf{
	class SmfPackage
		:public ObjectStatic
	{
	protected:
		unsigned _index = 0;
	public:
		bool Pack(Frame* frm);
		bool Pack(void* pack, uint32_t size,uint32_t flags);
		bool Unpack(Frame* frm);
		int Unpack(Frame* frm,void* data,int max);
		inline int Size()const { return sizeof(smf_packet_t); }
	};
}

