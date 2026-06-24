#pragma once
#include "ObjectStatic.h"
#include "SmfFifo.def.h"
namespace smf {
	class FifoBufferReader;
	class SmfFifoBuffer : public ObjectStatic {
	public:
		using fifo_t = smf_fifo_t;
	public:
		SmfFifoBuffer() {}
		SmfFifoBuffer(fifo_t* fifo, void* data = 0, uint32_t max = 0);
	protected:
		fifo_t* _fifo = 0;
	public:
		void Initialize(fifo_t* fifo, void* data = 0, uint32_t max = 0);
		void Uninit();
	public:
		bool IsValid()const { return _fifo && _fifo->data; }
		operator bool()const { return _fifo && _fifo->data; }
		fifo_t* Fifo()const { return (fifo_t*)_fifo; }
		void Fifo(fifo_t* fifo) { _fifo = fifo; }
		void* Data()const { return (void*)_fifo->data; }
		uint32_t Max()const { return _fifo->max; }
	public:
		int Size()const { return Size(_fifo->ri); }
		int Left()const { return Left(_fifo->ri); }
		bool Full() const { return Full(_fifo->ri); }
		bool Empty() const { return Empty(_fifo->ri); }
	public:
		uint64_t WriteIndex()const { return _fifo->wi; }
		uint32_t Write(const void* data, uint32_t size, bool all = false, bool cache = false, bool is_ring = false, bool monoblock = false, void** target = 0);
		uint32_t WriteWithCache(const void* data, uint32_t size, bool all = false, bool is_ring = false) { return Write(data, size, all, true, is_ring); }
		uint32_t WritePointer(void*& data, bool is_ring = false)const;
		uint32_t Fill(char chr, uint32_t count, bool cache = false, bool is_ring = false);
		uint32_t FillWithCache(char chr, uint32_t count, bool is_ring = false) { return Fill(chr, count, true, is_ring); }
		int WriteSeek(int offset, bool is_ring = false);
	public:
		uint64_t ReadIndex()const { return _fifo->ri; }
		uint32_t Read(void* data, uint32_t size, bool all = false, bool cache = false) { return Read(_fifo->ri, data, size, all, cache); }
		uint32_t ReadWithCache(void* data, uint32_t size, bool all = false) { return ReadWithCache(_fifo->ri, data, size, all); }
		uint32_t Peak(void* data, uint32_t size, bool all = false, bool cache = false)const { return Peak(_fifo->ri, data, size, all, cache); }
		uint32_t PeakWithCache(void* data, uint32_t size, bool all = false)const { return PeakWithCache(_fifo->ri, data, size, all); }
		uint32_t ReadPointer(void*& data, bool cache = false)const { return ReadPointer(_fifo->ri, data, cache); }
		int ReadSeek(int offset, bool check = true) { return ReadSeek(_fifo->ri, offset, check); }
		int ReadSync(int offset = 0, bool check = true) { return ReadSync(_fifo->ri, offset, check); }
		int ReadSyncLast() { return ReadSyncLast(_fifo->ri); }
		int ReadCheck(bool sync = false) { return ReadCheck(_fifo->ri, sync); }
	protected:
		int Size(uint64_t ri)const { return (int)(_fifo->wi - ri); }
		int Left(uint64_t ri)const { return (int)Max() - Size(ri); }
		bool Full(uint64_t ri) const { return ri + _fifo->max == _fifo->wi; }
		bool Empty(uint64_t ri) const { return ri == _fifo->wi; }
	public:
		int ReadSeek(uint64_t& ri, int offset, bool check = true) const;
		int ReadSync(uint64_t& ri, int offset = 0, bool check = true) const;
		int ReadSyncLast(uint64_t& ri) const { return ReadSync(ri, -(int)(_fifo->max)); }
		int ReadCheck(uint64_t& ri, bool sync = false) const;
		uint32_t Read(uint64_t& ri, void* data, uint32_t size, bool all, bool cache);
		uint32_t ReadWithCache(uint64_t& ri, void* data, uint32_t size, bool all) { return Read(ri, data, size, all, true); }
		uint32_t Peak(uint64_t& ri, void* data, uint32_t size, bool all, bool cache)const;
		uint32_t PeakWithCache(uint64_t& ri, void* data, uint32_t size, bool all) const { return Peak(ri, data, size, all, true); }
		uint32_t ReadPointer(uint64_t& ri, void*& data, bool cache = false)const;
	public:
		using Reader = FifoBufferReader;
		Reader CreateReader() const;
		Reader CreateReader(uint64_t ri) const;
	public:
		void* memalloc(uint32_t size);
		bool memfree(void* ptr, uint32_t size);
	};
	//
	class FifoBufferReader : protected SmfFifoBuffer {
	protected:
		mutable uint64_t _ri = 0;
	public:
		FifoBufferReader(fifo_t* fifo = 0, uint64_t ri = 0) {
			if (fifo)
				Initialize(fifo, ri);
		}
	public:
		void Initialize(fifo_t* fifo, uint64_t ri = 0) {
			_ri = ri; SmfFifoBuffer::Initialize(fifo);
		}
	public:
		using SmfFifoBuffer::IsValid;
		using SmfFifoBuffer::WriteIndex;
		using SmfFifoBuffer::Fifo;
		using SmfFifoBuffer::Uninit;
	public:
		uint64_t Index()const { return _ri; }
		int Size()const { return SmfFifoBuffer::Size(_ri); }
		int Left()const { return SmfFifoBuffer::Left(_ri); }
		bool Full()const { return SmfFifoBuffer::Full(_ri); }
		bool Empty()const { return SmfFifoBuffer::Empty(_ri); }
		int Check(bool sync = false) { return SmfFifoBuffer::ReadCheck(_ri, sync); }
	public:
		int ReadSync(int offset = 0, bool check = true) {
			return SmfFifoBuffer::ReadSync(offset, check);
		}
		int Seek(int offset, bool check = true) {
			return SmfFifoBuffer::ReadSeek(_ri, offset, check);
		}
		uint32_t Read(void* data, uint32_t size, bool all = false, bool cache = false) {
			return SmfFifoBuffer::Read(_ri, data, size, all, cache);
		}
		uint32_t Peak(void* data, uint32_t size, bool all = false, bool cache = false)const {
			return SmfFifoBuffer::Peak(_ri, data, size, all, cache);
		}
		uint32_t ReadPointer(void*& data, bool cache = false)const {
			return SmfFifoBuffer::ReadPointer(_ri, data, cache);
		}
		void CheckAndAjustRi(int align = 0);
	};
	//
	class SmfFifoBufferX
		: public SmfFifoBuffer
	{
	public:
		SmfFifoBufferX(void* data, uint32_t max, uint64_t wi, uint64_t ri)
			:_fifox{ (uint64_t)data,wi,ri,max,0ul }
		{
			SmfFifoBuffer::Initialize(&_fifox);
		}
	protected:
		using SmfFifoBuffer::Initialize;
	protected:
		fifo_t _fifox;
	};
	//
	template<int _max, int _align = 0>
	class SmfFifoBufferS
		: public SmfFifoBuffer
	{
	public:
		SmfFifoBufferS() {
			auto buff = _align ? (char*)(((uint32_t)_buff + _align - 1) / _align * _align) : _buff;
			SmfFifoBuffer::Initialize(&_fifoS, buff, _max);
		}
	protected:
		using SmfFifoBuffer::Initialize;
		//using SmfFifoBuffer::Uninit;
	protected:
		fifo_t _fifoS;
		char _buff[_max + _align];
	};
	//
	class SmfFifoBufferD
		: public SmfFifoBuffer
	{
	public:
		SmfFifoBufferD(int max = 0, int align = 0);
		~SmfFifoBufferD();
	protected:
		using SmfFifoBuffer::Initialize;
	public:
		bool Initialize(uint32_t max, int align = 0);
		void Uninit();
	protected:
		fifo_t _fifoD;
	};
}

