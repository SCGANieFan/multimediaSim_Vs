#include "IPool.h"
#include <stdint.h>
#include <functional>
namespace smf {
	class PoolTable : public IPool{
	public:
		PoolTable(bool record = true);
		virtual ~PoolTable();
	public:
		typedef struct {
			uint16_t size;
			uint16_t num;
		}item_t;
		bool Initialize(void*& buff, int size, item_t*);
		bool Initialize(item_t*,IPool*pool=0);
		int GetBuffSize(item_t*);
	protected:
		virtual void* alloc(unsigned size, unsigned align)override;
		//virtual void* realloc(void* ptr, unsigned size)override;
		//virtual void* calloc(unsigned num, unsigned size)override;
		//virtual void* alloc_align(unsigned size, int align)override;
		virtual bool free(void* ptr)override;
		//virtual void print()const override;
		virtual char* print(char* ptr, char* end) const override;
		virtual int get_free()const override;
		virtual int get_total()const override;
		virtual int get_size(void* ptr)const override;
		virtual bool is_valid()const override;
	protected:
		class CItem{
		public:
			uint16_t _blksize=0;
			uint16_t _blknum=0;
			char* _buff=0;
			uint32_t* _maps=0;
		public:
			bool Initialize(char*& buff, char* buff_end, int blksize, int blknum);
		public:
			void* Alloc();
			bool Free(void*);
			bool IsValid(void*)const;
			uint16_t FreeBlockNum()const;
			uint32_t FreeSize()const;
			uint32_t Size()const;
			void Print()const;
			char* Print(char* ptr, char* end)const;
		};
	protected:
		IPool* _pool = 0;
		CItem* _items = 0;
		uint32_t _count = 0;
	protected:
		void foreach(const std::function<bool(CItem&)>& func);
	};
}
