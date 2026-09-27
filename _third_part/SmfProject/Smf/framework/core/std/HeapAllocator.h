#pragma once
#include "SimpleHeap.h"
#include "Allocator.h"
namespace smf {
	template <class T>
	class HeapAllocator : public TAllocator<T, SimpleHeap> {
	private:
		using Base = TAllocator<T, SimpleHeap>;
	public:
		constexpr HeapAllocator(void* buff, int size, int thr = 0) noexcept
			: Base(SimpleHeap(buff, size, thr))
		{	
		}
		constexpr HeapAllocator(const HeapAllocator&) noexcept = default;
		HeapAllocator& operator=(const HeapAllocator&) = default;
	public:
		void Set(void* buff, int size, int thr = 0)noexcept {
			Base::_x.Initialize(buff,size,thr);
		}
	};
	//
	template<class K, class V>
	using MapHeapAllocator = HeapAllocator<std::pair<const K, V>>;
}