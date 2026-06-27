#include "IPool.h"

namespace smf {
	class PoolList : public IPool{
	public:
		PoolList(IPool**pools,uint32_t max);
		virtual ~PoolList();
	public:
		bool Add(IPool*);
		bool Remove(IPool*);
	protected:
		virtual void* alloc(unsigned size, unsigned align)override;
		virtual void* realloc(void* ptr, unsigned size)override;
		//virtual void* calloc(unsigned num, unsigned size)override;
		//virtual void* alloc_align(unsigned size, unsigned align)override;
		virtual bool free(void* ptr)override;
		//virtual void print()const override;
		virtual char* print(char* ptr, char* end) const override;
		virtual int get_free()const override;
		virtual int get_total()const override;
		virtual int get_size(void* ptr)const override;
		virtual bool is_valid()const override;
	protected:
		IPool** _pools = 0;
		uint32_t _count = 0;
	protected:
		IPool* find(void*) const;
	};

	template<uint32_t __max>
	class TPoolListX : public PoolList {
	public:
		TPoolListX() :PoolList(_poolx, __max) {}
		template<class P>
		TPoolListX(P ps[__max]) :PoolList(_poolx, __max) {
			for (int i = 0; i < __max; i++)
				_poolx[i] = &ps[i];
		}
	private:
		IPool* _poolx[__max];
	};
}
