#pragma once
#include "IPool.h"
#include "smf_debug.h"
#include "memory.hh"
#include <utility>
namespace smf {
	template<class X>
	class TVoidPtr {
	private:
		using This = TVoidPtr<X>;
	public:
		TVoidPtr() {}
		TVoidPtr(const This& obj) { reset(obj); }
		TVoidPtr(This&& obj) noexcept = default;
		explicit TVoidPtr(void* data, int size = 0) :_data(data) {}
		explicit TVoidPtr(int size, int align = 0) { resetX(0, size, align); }
		~TVoidPtr() { resetX(0, 0, 0); }
	protected:
		void* _data = 0;
	private:
		void resetX(const void* data, int size, int align, bool hold = false) {X::ResetX(this,data,size,align,hold);}
	public:
		void reset(const void* data, int size = 0) { resetX(data, size, 0); }
		void reset(int size = 0, int align = 0) { resetX(0, size, align); }
		void reset(const This& obj) { resetX(obj._data, 0, 0, true); }
		void duplicate(const void* data, int size, int align = 0) {resetX(0, size, align);if (_data) {memcpy(_data, data, size);}}
		template<class T = void>
		T* release() { auto data = (T*)_data;  _data = 0;  return data; }
		void swap(This& obj) { std::swap(_data, obj._data); }
		operator bool()const { return (bool)_data; }
		operator void* ()const { return (void*)_data; }
		template<class T>
		operator T* ()const { return (T*)_data; }
		template<class T = void>
		T* get()const { return (T*)_data; }

		This& operator=(This&& obj) {swap(obj); return *this;}//noexcept = default;
		This& operator=(const This& obj) { reset(obj); return *this; }
		This& operator=(void* obj) { reset(obj); return *this; }
	protected:
		friend class XVoidPtr;
		friend class XPoolPtr;
		friend class XVoidBuffer;
		friend class XPoolBuffer;
	};

	template<class X>
	class TPoolPtr : public TVoidPtr<X> {
	private:
		using Base = TVoidPtr<X>;
		using This = TPoolPtr<X>;
	public:
		using Base::Base;
		TPoolPtr(This&& obj) noexcept { swap(obj); }
		TPoolPtr(const This& obj) noexcept { reset(obj); }
		explicit TPoolPtr(IPool* pool) :_pool(pool) {}
		explicit TPoolPtr(IPool* pool, void* data, int size = 0) { reset(pool, data, size); }
		explicit TPoolPtr(IPool* pool, int size, int align = 0) { reset(pool, size, align); }
	public:
		This& operator=(This&& obj) {swap(obj); return *this;}
		This& operator=(const This& obj) { reset(obj); return *this; }
	protected:
		IPool* _pool = 0;
	public:
		IPool* pool()const { return (IPool*)_pool; }
		using Base::reset;
		void reset(IPool* pool) { Base::reset(); _pool = pool; }
		void reset(IPool* pool, void* data, int size = 0) { reset(pool); Base::reset(data, size); }
		void reset(IPool* pool, int size, int align = 0) { reset(pool); Base::reset(size, align); }
		void reset(const This& obj) { reset(obj._pool); Base::reset((const Base&)obj); }
		using Base::swap;
		void swap(This& obj) { Base::swap(obj); std::swap(_pool, obj._pool); }
	protected:
		friend class XVoidPtr;
		friend class XPoolPtr;
		friend class XVoidBuffer;
		friend class XPoolBuffer;
	};

	template<class Base>
	class TxBuffer
		: public Base
	{
	private:
		using This = TxBuffer<Base>;
	public:
		using Base::Base;
		TxBuffer(TxBuffer&& buff) { swap(buff); }
		TxBuffer(const TxBuffer& buff) { reset(buff); }
		explicit TxBuffer(void* data, int size = 0) :Base(data), _size(size) {}
	protected:
		int _size = 0;
	public:
		int size()const { return _size; }
		using Base::reset;
		void reset(const void* data, int size) { Base::reset(data,size); _size = size; }
		void reset(int size = 0, int align = 0) { Base::reset(size, align); _size = size; }
		void reset(const This& obj) { Base::reset((const Base&)obj); _size = obj._size; }
		void duplicate(const void* data, int size, int align = 0) { Base::duplicate(data, size, align); _size = size; }
		using Base::swap;
		void swap(This& obj) { Base::swap(obj); std::swap(_size, obj._size); }
		using Base::operator=;
		This& operator=(This&& buff) { swap(buff); return *this; }
		This& operator=(const This& buff) { reset(buff); return *this; }
	protected:
		friend class XVoidPtr;
		friend class XPoolPtr;
		friend class XVoidBuffer;
		friend class XPoolBuffer;
	};

	class XVoidPtr {
	public:
		static void ResetX(void* this_, const void* data, int size, int align, bool hold);
	};
	class XVoidBuffer {
	public:
		static void ResetX(void* this_, const void* data, int size, int align, bool hold);
	};
	class XPoolPtr {
	public:
		static void ResetX(void* this_, const void* data, int size, int align, bool hold);
	};
	class XPoolBuffer {
	public:
		static void ResetX(void* this_, const void* data, int size, int align, bool hold);
	};

	using VoidPtr = TVoidPtr<XVoidPtr>;
	using PoolPtr = TPoolPtr<XPoolPtr>;
	using VoidBuffer = TxBuffer<TVoidPtr<XVoidBuffer>>;
	using PoolBuffer = TxBuffer<TPoolPtr<XPoolBuffer>>;
}
