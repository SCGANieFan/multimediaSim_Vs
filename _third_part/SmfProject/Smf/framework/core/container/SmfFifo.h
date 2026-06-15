#pragma once
#include "ObjectStatic.h"
#include "SmfFifo.def.h"

namespace smf {
	class SmfFifoBase: public ObjectStatic{
	public:
		using fifo_t = smf_fifo_t;
		typedef void (*CbCopy)(void* dst, const void* src);
	public:
		SmfFifoBase(fifo_t* fifo = 0, void* data = 0, int size = 0) { if (fifo)Initialize(fifo, data, size); }
	protected:
		fifo_t* _fifo = 0;
	public:
		void Initialize(fifo_t* fifo, void* data = 0, int max = 0);
	public:
		bool IsValid()const { return _fifo && _fifo->data; }
		operator bool()const { return IsValid(); }
		fifo_t* Fifo()const { return (fifo_t*)_fifo; }
		void Fifo(fifo_t* fifo) { _fifo = fifo; }
		uint64_t WriteIndex()const { return _fifo->wi; }
		uint64_t ReadIndex()const { return _fifo->ri; }
		uint32_t Max()const { return _fifo->max; }
		void* Data()const { return (void*)_fifo->data; }
		int Size()const {return (uint32_t)(_fifo->wi - _fifo->ri);}
		uint32_t Left()const {return Max() - Size();}
		bool Full() const { return _fifo->ri + _fifo->max == _fifo->wi; }
		bool Empty() const { return _fifo->ri == _fifo->wi; }
	public://writer
		bool Write(const void* src, int size, CbCopy cb, bool cache, bool is_ring);
		void* Peak(uint64_t ri, int offset, int size) const;
		bool Read(uint64_t& ri, void* dst, int size, CbCopy cb, bool cache);
		int ReadSeek(uint64_t& ri, int offset);
		int ReadSync(uint64_t& ri, int offset);
		void Clean() const {_fifo->ri = _fifo->wi; }
	public:
		int ReadSeek(int offset) { return ReadSeek(_fifo->ri, offset); }
		int ReadSync(int offset) { return ReadSync(_fifo->ri, offset); }
		int WriteSeek(int offset, bool is_ring = false);
	protected:
		template<class T>static inline 
		void copy(void* dst, const void* src) {*(T*)dst = *(T*)src;}
	};
	//
	template<class T>
	class SmfFifo :public SmfFifoBase {
	public:
		using SmfFifoBase::SmfFifoBase;
		using SmfFifoBase::Write;
		using SmfFifoBase::Read;
		using SmfFifoBase::Peak;
	public://writer
		bool Write(const T& t, bool cache = false, bool is_ring = false, CbCopy cb = 0) {
			return SmfFifoBase::Write(&t, sizeof(T), cb ? cb : &copy<T>, cache, is_ring);
		}
	public://reader
		T* Peak(int offset = 0) const {
			return (T*)SmfFifoBase::Peak(_fifo->ri, offset, sizeof(T));
		}
		bool Read(T&t, bool cache = false, CbCopy cb = 0) {
			return SmfFifoBase::Read(_fifo->ri, &t, sizeof(T), cb ? cb : &copy<T>, cache);
		}
	};
	//
	template<class T>
	class SmfFifoReader :protected SmfFifoBase {
	public:
		SmfFifoReader(fifo_t* fifo, uint64_t ri) :SmfFifoBase(fifo),_ri(ri) {}
		void Initialize(fifo_t* fifo, uint64_t ri = 0) { SmfFifoBase::Initialize(fifo, 0, 0); _ri = ri; }
	protected:
		uint64_t _ri = 0;
	public://reader
		T* Peak(int offset = 0) const {return (T*)SmfFifoBase::Peak(_ri, offset, sizeof(T));}
		bool Read(T& t, bool cache = false) {return SmfFifoBase::Read(_ri, &t, sizeof(T), &copy<T>, cache);}
		uint32_t ReadSeek(int offset) { return ReadSeek(_ri, offset); }
	public:
		using SmfFifoBase::Data;
		using SmfFifoBase::WriteIndex;
		using SmfFifoBase::WriteSeek;
		using SmfFifoBase::Max;
		using SmfFifoBase::IsValid;
		using SmfFifoBase::Fifo;
	public:
		int Size()const { return (uint32_t)(_fifo->wi - _ri); }
		int Left()const { return Max() - Size(); }
		bool Full() const { return _ri + _fifo->max == _fifo->wi; }
		bool Empty() const { return _ri == _fifo->wi; }
		uint64_t ReadIndex()const { return _ri; }
	};
	//
	template<class T>
	class SmfFifoX :public SmfFifo<T> {
	private:
		smf_fifo_t _fifox;
	public:
		SmfFifoX(void* data, uint32_t max, uint64_t wi, uint64_t ri)
			:_fifox{ (uint64_t)data,wi,ri,max,0ul }
		{
			SmfFifo<T>::Initialize(&_fifox);
		}
	};
	//
	template<class T, unsigned _max>
	class SmfFifoS :public SmfFifo<T> {
	protected:
		smf_fifo_t _fifo;
		T _data[_max];
	public:
		SmfFifoS() :SmfFifo<T>(&_fifo, _data, _max) {}
	};
	//
	template<class T>
	class SmfFifoD : public SmfFifo<T> {
	protected:
		smf_fifo_t _fifo;
	public:
		SmfFifoD(int max = 0) { _fifo.data = 0; Initialize(max); }
		~SmfFifoD() { Uninit(); }
	public:
		bool Initialize(int max) {
			SmfFifo<T>::Initialize(&_fifo, (max ? Calloc(max, sizeof(T)) : 0), max);
			return this->IsValid();
		}
		void Uninit() {
			Free((void*&)_fifo.data);
			_fifo.data = 0;
		}
	};
}
