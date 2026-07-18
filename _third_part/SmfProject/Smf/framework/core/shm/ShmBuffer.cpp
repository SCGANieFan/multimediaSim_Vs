#include "ShmBuffer.h"
#include "SmfHash.h"
#include "SmfDef.h"
#include "IPool.h"
#include "smf_debug.h"
#include "SmfPorting.h"
#include "MediaAudio.h"
using namespace smf;
#define MAGIC 0x55aa5aa5
ShmBuffer* ShmBuffer::Create(const char* name
	, uint32_t blksize, uint32_t blknum
	, uint32_t blockMs, uint32_t blockMsDenum
	, void* media, uint32_t mediaSize
	, bool cacheable
) {
	dbgTestPXL("%s,%u*%u,%u/%u,%p/%u,%u",name
		,blksize,blknum
		,blockMs,blockMsDenum
		, media, mediaSize
		, cacheable
	);
	blksize = (blksize + 63) >> 6 << 6;//align 64
	auto size = sizeof(_head) + blksize * blknum;
	auto shm = (ShmBuffer*)IPool::Shared().Alloc(name, size, 64);
	returnIfErrC(0, !shm);
	dbgTestPXL("%p,%u",shm,size);
	if (cacheable) { cache_invalid(shm, size); }
	//if (shm->_name == fcc64(name)
	//	&& shm->_size == size
	//	&& shm->_blksize == blksize
	//	&& shm->_blknum == blknum
	//	) {
	//	dbgTestPL();
	//	return shm;
	//}
	//memset(shm, 0, size);
	//if (cacheable) { cache_invalid(this, sizeof(_head) + sizeof(_blks)); }
	shm->_magic = 0x55aa5aa5;
	shm->_name = fcc64(name);
	shm->_size = size;
	shm->_blksize = blksize;
	shm->_blknum = blknum;
	if(blockMs)shm->_blockMs = blockMs;
	if(blockMsDenum)shm->_blockMsDenum = blockMsDenum;
	if(mediaSize && media) {
		shm->_mediaSize = mediaSize;
		memcpy((char*)shm->_media, media, mediaSize);
	}
	if(!shm->_timePointer)shm->_timePointer = get_ms();
	if (cacheable) { cache_writeback(shm, size); }
	return shm;
}
void ShmBuffer::Destroy(ShmBuffer*shm) {
	IPool::Shared().Free(shm);
}

void* ShmBuffer::Get(uint32_t idx, bool cacheable) {
	if (cacheable) cache_invalid((void*)_head, _size);
	return (char*)_buff + _blksize * (idx % _blknum);
}

void* ShmBuffer::Get(uint32_t idx, uint32_t& size, bool cacheable) {
	if (cacheable) cache_invalid((void*)_head, _size);
	size = _blksize;
	//dbgTestPXL("%p,%u,%u,%u,%p", _buff, _blksize, idx, _blknum, (char*)_buff + _blksize * (idx % _blknum));
	return (char*)_buff + _blksize * (idx % _blknum);
}

void ShmBuffer::Flush(void* data, bool cacheable) {
	if(cacheable)cache_flush(data, _blksize);
}

bool ShmBuffer::Valid(bool cacheable)const {
	if (cacheable) cache_invalid((void*)_head, sizeof(_head));
	return (_magic == MAGIC) && _name && _size 
		&& _blksize && _blknum
		&& _blockMs && _blockMsDenum
		;
}

const ShmBuffer::head_t* ShmBuffer::Head(bool cacheable)const {
	if (cacheable) cache_invalid((void*)_head, sizeof(_head));
	return (head_t*)_head;
}

bool ShmBuffer::Set(void* media, uint32_t mediaSize, bool cacheable) {
	if (cacheable) cache_invalid((void*)_head, sizeof(_head));
	memcpy((char*)_media, media, mediaSize);
	_mediaSize = mediaSize;
	if (cacheable) cache_writeback((void*)_head, sizeof(_head));
	return true;
}

bool ShmBuffer::Set(uint32_t blockMs, uint32_t blockMsDenum, bool cacheable) {
	if (cacheable) cache_invalid((void*)_head, sizeof(_head));
	_blockMs = blockMs;
	_blockMsDenum = blockMsDenum;
	if (cacheable) cache_writeback((void*)_head, sizeof(_head));
	return true;
}

uint32_t ShmBuffer::GetIndex(bool cacheable) const {
	if (cacheable) cache_invalid((void*)_head, sizeof(_head));
	auto ms = get_ms();
	uint64_t diff = 0;
	if (ms >= _timePointer) {
		diff = ms - _timePointer;
	}
	else {
		diff = 0x100000000ull + ms - _timePointer;
	}
	auto idx = (uint32_t)((diff / _blksize)% _blknum);
	idx %= _blknum;
	dbgTestPXL("",ms, _timePointer,idx);
	return idx;
}
