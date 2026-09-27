#pragma once
#include "IPool.h"
#include "Allocator.h"
namespace smf {
	template <class T>
	class PoolAllocator : public TPtrAllocator<T, IPool> {
	private:
		using Base = TPtrAllocator<T, IPool>;
	public:
		constexpr PoolAllocator(const char* poolname) noexcept
			: Base(Pools().Find(poolname))
		{
		}
		constexpr PoolAllocator(const PoolAllocator&) noexcept = default;
		PoolAllocator& operator=(const PoolAllocator&) = default;
	public:
		void Set(const char* poolname)noexcept {
			Base::Set(Pools().Find(poolname));
		}
	};
	//
	template<class K, class V>
	using MapPoolAllocator = PoolAllocator<std::pair<const K, V>>;
}