#include "SmfFifo.h"
#include "SmfMsg.h"

namespace smf {
	class ShmMaps {
	public:
		using XMsg = msg::XMsg<256>;
		using XFifo = SmfFifo<XMsg>;
	protected:
		smf_fifo_t* _fifo[8][8];
	public:
		ShmMaps();
		ShmMaps(void* addr, int size, int fifomax, uint32_t cpumaps);
	public:
		bool Initialize(void* addr, int size, int fifomax, uint32_t cpumaps);
		bool Push(smf_msg_t* msg);
		bool Pull(void(*cb)(smf_msg_t* msg,void*priv), void* priv);
	public:
		static constexpr int Total(int fifomax, uint32_t cpumaps) {
			int c = 0;
			auto cpumap = cpumaps;
			while (cpumap) {
				if (cpumap & 1)c++;
				cpumap >>= 1;
			}
			return (fifomax * sizeof(XMsg) + sizeof(smf_fifo_t)) * c * c;
		}
		static int GetSize(int fifomax, uint32_t cpumaps);
		static ShmMaps* Create(void* addr, int size, int fifomax, uint32_t cpumaps);
		static void* operator new(size_t size, void* ptr) { return ptr; }
	};
	template<int fifomax,uint32_t cpumaps>
	class TShmMaps : public ShmMaps {
	public:
		TShmMaps():ShmMaps(_buff,sizeof(_buff),fifomax,cpumaps){}
	protected:
		char _buff[Total(fifomax,cpumaps)];
	};
}
