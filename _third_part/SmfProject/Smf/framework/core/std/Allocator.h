#pragma once
#include "SmfDef.h"

namespace smf {
	template <class T>
	using XAllocator = std::allocator<T>;

	//template <class T>
	//class XAllocator{
	//public:
	//	using _From_primary = XAllocator;
	//	using value_type = T;
	//	using size_type = size_t;
	//	using difference_type = ptrdiff_t;
	//	using propagate_on_container_move_assignment = std::true_type;
	//};
	class TempAlloc {
	public:
		static void* Alloc(uint32_t size) { return 0; }
		static void Free(void* ptr) { }
	};

	template <class T>
	class BufferAllocator : public XAllocator<T> {
	public:
		using XAllocator<T>::XAllocator;
		constexpr BufferAllocator(void*buff,int size) noexcept
			:_buff((char*)buff)
			,_ptr(_buff)
			,_end(_buff + size)
		{			
		}
		constexpr BufferAllocator(const BufferAllocator&) noexcept = default;
		BufferAllocator& operator=(const BufferAllocator&) = default;
	public:
		void deallocate(T* const _Ptr, const size_t _Count) {
		}

		T* allocate(const size_t _Count){
			auto ptr = _ptr;
			_ptr += sizeof(T) * _Count;
			return (T*)(_ptr <= _end ? ptr : 0);
		}
	public:
		char* _buff = 0;
		char* _ptr = 0;
		char* _end = 0;
		template <typename U>
		struct rebind {
			using other = BufferAllocator<U>;
		};
	};

	template <class T>
	class CallbackAllocator : public XAllocator<T> {
	public:
		typedef void* (*CbAlloc)(uint32_t);
		typedef void (*CbFree)(void*);
	public:
		using XAllocator<T>::XAllocator;
		constexpr CallbackAllocator(CbAlloc cbAlloc, CbFree cbFree = 0) noexcept 
			:_cbAlloc(cbAlloc)
			, _cbFree(cbFree)
		{			
		}
		constexpr CallbackAllocator(const CallbackAllocator&) noexcept = default;
		CallbackAllocator& operator=(const CallbackAllocator&) = default;
	public:
		void deallocate(T* const _Ptr, const size_t _Count) {
			if (_cbFree)_cbFree(_Ptr);
		}

		T* allocate(const size_t _Count) {
			return (T*)(_cbAlloc ? _cbAlloc(sizeof(T) * _Count) : 0);
		}
	public:
		CbAlloc _cbAlloc = 0;
		CbFree _cbFree = 0;
		template <typename U>
		struct rebind {
			using other = CallbackAllocator<U>;
		};
	};

	template <class T, class X>
	class TPtrAllocator : public XAllocator<T> {
	public:
		using XAllocator<T>::XAllocator;
		constexpr TPtrAllocator(X* x) noexcept
			: _x(x)
		{
		}
		constexpr TPtrAllocator(const TPtrAllocator&) noexcept = default;
		TPtrAllocator& operator=(const TPtrAllocator&) = default;
	public:
		void deallocate(T* const _Ptr, const size_t _Count) {
			auto ptr = (T*)_Ptr;
			if (_x)_x->Free(ptr);
		}

		T* allocate(const size_t _Count) {
			return (T*)(_x ? _x->Alloc(sizeof(T) * _Count) : 0);
		}
	public:
		X* _x = 0;
		template <typename U, typename W>
		struct rebind {
			using other = TPtrAllocator<U,W>;
		};
	};

	template <class T, class X>
	class TAllocator : public XAllocator<T> {
	public:
		using XAllocator<T>::XAllocator;
		constexpr TAllocator(X&& x) noexcept
			: _x(x)
		{
		}
		constexpr TAllocator(const X& x) noexcept
			: _x(x)
		{
		}
		constexpr TAllocator(const TAllocator&) noexcept = default;
		TAllocator& operator=(const TAllocator&) = default;
	public:
		void deallocate(T* const _Ptr, const size_t _Count) {
			auto ptr = (T*)_Ptr;
			_x.Free(ptr);
		}

		T* allocate(const size_t _Count) {
			return (T*)_x.Alloc(sizeof(T) * _Count);
		}
	public:
		X _x;
		template <typename U, typename W>
		struct rebind {
			using other = TAllocator<U,W>;
		};
	};

	template <class T>
	class GlobleAllocator : public XAllocator<T> {
	public:
		using XAllocator<T>::XAllocator;
		constexpr GlobleAllocator() noexcept = default;
		constexpr GlobleAllocator(const GlobleAllocator&) noexcept = default;
		GlobleAllocator<T>& operator=(const GlobleAllocator&) = default;
	public:
		void deallocate(T* const _Ptr, const size_t _Count) {
			T* ptr = (T*)_Ptr;
			smf::Free(ptr);
		}
		T* allocate(const size_t _Count) {
			return (T*)smf::Alloc(sizeof(T) * _Count);
		}
		template <typename U>
		struct rebind {
			using other = GlobleAllocator<U>;
		};
	};

	//
	template<class K, class V>
	using MapBufferAllocator = BufferAllocator<std::pair<const K, V>>;
	//
	template<class K, class V>
	using MapCallbackAllocator = CallbackAllocator<std::pair<const K, V>>;
	//
	template<class K, class V, class X>
	using MapTAllocator = TAllocator<std::pair<const K, V>, X>;
	//
	template<class K, class V, class X>
	using MapTPtrAllocator = TPtrAllocator<std::pair<const K, V>, X>;
	//
	template<class K, class V, class X>
	using MapGlobleAllocator = GlobleAllocator<std::pair<const K, V>>;
}
