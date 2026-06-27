#pragma once
#include "IPoolShared.h"
#include "IPool.h"
#include "mutex.h"
namespace smf{
	class MemoryNamed
		: public IPoolShared
	{
	public:
		struct item_t {
			uint32_t hash;
			uint32_t size:20;
			uint32_t cnt:4;
			uint32_t offset:8;
			char buff[0];

			void* ptr() { return buff + offset; }
		};
	protected:
		mutex _mtx;
		item_t** _items = 0;
		int _itemsCnt = 0;
		IPool* _pool = 0;
	public:
		virtual ~MemoryNamed();
	public:
		using IPoolShared::Init;
		bool Init(IPool* pool, int cnt);
	protected:
		virtual void* alloc(const char* name, void* buff, unsigned& size, unsigned align)override;
		virtual bool free(const char* name, void* buff)override;
	protected:
		item_t** find(uint32_t hash);
		item_t** find(void* buff);
		item_t* allocX(uint32_t hash, unsigned size, unsigned align);
		void* alloc(uint32_t hash, unsigned size, unsigned align);
		void* alloc(uint32_t hash, unsigned& size);
		void* alloc(void* buff, unsigned& size);
		bool free(item_t** item);
	public:
		virtual bool IsLocal()const override { return true; }
	};
}
