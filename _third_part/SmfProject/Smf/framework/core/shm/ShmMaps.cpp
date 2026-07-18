#include "ShmMaps.h"
#include "smf_debug.h"
using namespace smf; 
using namespace smf::msg; 

ShmMaps::ShmMaps() {
	memset(_fifo, 0, sizeof(_fifo));
}
ShmMaps::ShmMaps(void* addr, int size, int fifomax, uint32_t cpumaps) {
	memset(_fifo, 0, sizeof(_fifo));
	returnIfErrC0(Initialize(addr, size, fifomax, cpumaps));
}

bool ShmMaps::Initialize(void* addr, int size, int fifomax, uint32_t cpumaps) {
	dbgTestPXL("%p,%d,%d,%08x", addr, size, fifomax, cpumaps);
	//auto request = GetSize(fifomax, cpumaps);
	//returnIfErrCS(false, size < request, "%d<%d", size, request);
	memset(addr, 0, size);
	auto data = (char*)addr;
	auto flen = fifomax * sizeof(XMsg) + sizeof(smf_fifo_t);
	for (int8_t i = 0; i < 8; i++) {
		if (cpumaps & (1u << i)) {
			for (int8_t j = 0; j < 8; j++) {
				if (cpumaps & (1u << j)) {
					auto& fifo = _fifo[i][j];
					fifo = (smf_fifo_t*)data; data += sizeof(smf_fifo_t);
					fifo->data = (uint64_t)data; data += flen;
					fifo->max = fifomax;
				}
			}
		}
	}
	return true;
}

bool ShmMaps::Push(smf_msg_t* msg) {
	returnIfErrC(false, msg->cpudst > 8);
	returnIfErrC(false, msg->cpusrc > 8);
	auto fifox = _fifo[msg->cpudst - 1][msg->cpusrc - 1];
	XFifo fifo(fifox);
	dbgTestPXL("send:%p,%d,%d,%03x,%d,%p:%d/%d/%d", fifox, msg->cpudst, msg->cpusrc, msg->cmd, msg->size, fifox->data, (int)fifox->ri, (int)fifox->wi, fifox->max);
	//dbgTestDump(msg, msg->size);
	return fifo.Write((const void*)msg, sizeof(XMsg), (SmfFifoBase::CbCopy)[](void* dst, const void* src) {
		memcpy(dst, src, ((smf_msg_t*)src)->size);
		}, false, false);
}

bool ShmMaps::Pull(void(*cb)(smf_msg_t* msg, void* priv), void* priv) {
	auto srcid = CpuID();
	for (auto fifox : _fifo[srcid - 1]) {
		//for (int i = 0; i < 4; i++) {
		//auto& fifox = shm->fifo[srcid - 1][i];
		if (fifox && fifox->data) {
			XMsg msg;
			XFifo fifo(fifox);
			while (fifo && fifo.Read(msg)) {
				//dbgTestDump(&msg, msg.size?msg.size:32);
				//scv->Receive(msg);
				cb(&msg, priv);
			}
		}
	}
	return true;
}

int ShmMaps::GetSize(int fifomax, uint32_t cpumaps) {
	int c = 0;
	auto cpumap = cpumaps;
	while (cpumap) {
		if (cpumap & 1)c++;
		cpumap >>= 1;
	}
	return (fifomax * sizeof(XMsg) + sizeof(smf_fifo_t)) * c * c;
}

ShmMaps* ShmMaps::Create(void* addr, int total, int fifomax, uint32_t cpumaps) {
	auto request = GetSize(fifomax, cpumaps) + sizeof(ShmMaps);
	returnIfErrCS(0, total < request, "%d<%d", total, request);
	auto data = (char*)addr + sizeof(ShmMaps);
	auto size = total - sizeof(ShmMaps);
	auto maps = new(addr) ShmMaps();
	returnIfErrC(0, !maps->Initialize(data, size, fifomax, cpumaps));
	return maps;
}
