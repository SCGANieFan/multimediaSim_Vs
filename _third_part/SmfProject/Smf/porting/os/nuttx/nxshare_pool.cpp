// #include "MemoryPoolHeap.h"
#include "smf_debug.h"
#include <stdlib.h>
#include <assert.h>
#include "smf_api.h"

EXTERNC int bes_global_shmem_get_audio_shmem(unsigned int* addr, unsigned int* len);
namespace smf {
	// IPool* _nx_share_pool = 0;
	bool smf_nxshare_pool_register() {
		// unsigned int * buff = 0x39e00000;
		// unsigned int size = 1024 * 1024 * 1;  //0x39e00000 到0x3a000000 地址空间做数据共享
		unsigned char* buff = 0;
		unsigned int size = 0;
		unsigned int addr = 0;
		bes_global_shmem_get_audio_shmem(&addr, &size);
		buff = (unsigned char*)addr;
		int res_size = 1024 * 1024 * 2 + 10240; //2048 for smf_shm
		buff += res_size;
		size -= res_size;

		// buff = (unsigned int *)0x3A000000;
		// size = 1024 * 256;

		dbgInfoPXL("nxshare memory:0x%08x/0x%08x, 0x%08x", buff, buff + size, size);
		returnIfErrC(false, !buff);
		returnIfErrC(false, !size);
		// returnIfErrC(false, _nx_share_pool);
		// static MemoryPoolHeap pool;
		// pool.Name("nxshare");
		// returnIfErrC(false, !pool.Initialize(buff, size, 0));
		// _nx_share_pool = &pool;

		smf_register_pool("nxshare", buff, size, 1024 * 16);

		dbgTestPL();
		return true;
	}

	void smf_nxshare_pool_unregister() {
		// delete _nx_share_pool;
	}
}