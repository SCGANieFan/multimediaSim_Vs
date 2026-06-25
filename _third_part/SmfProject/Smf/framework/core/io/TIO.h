#pragma once
#include "IO.h"
namespace smf {
	template<class T>
	class TIORead
		: public IO
	{
	public:
		TIORead(T* t) :_t(t) {}
	public:///status ctrl
		virtual bool Open(void* para) override { return _t && _t->Open(para) && IO::Open(para); }
		virtual bool Close() override  { if (_t)_t->Close(); return IO::Close(); }
		virtual unsigned Read(void* buff, unsigned size) override { return _t?_t->Read(buff, size):0; }
	protected:
		T* _t = 0;
	};

	template<class T>
	class TIOWrite: public IO	{
	public:
		TIOWrite(T* t) :_t(t) {}
	public:///status ctrl
		virtual bool Open(void* para) override { return _t && _t->Open(para) && IO::Open(para); }
		virtual bool Close() override { return IO::Close() && _t && _t->Close();}
		virtual unsigned Write(void* buff, unsigned size) override { return _t?_t->Write(buff, size):0; }
	protected:
		T* _t = 0;
	};

	template<class CBOpen,class CBClose,class CBRead>
	class TIOCbRead: public IO	{
	public:
		TIOCbRead(CBOpen cbopen, CBClose cbclose, CBRead cbread) :_open(cbopen),_close(cbclose),_read(cbread) {}
	public:///status ctrl
		virtual bool Open(void* para) override { return _open && _open(para) && IO::Open(para); }
		virtual bool Close() override { return IO::Close() && _close && _close(); }
		virtual unsigned Read(void* buff, unsigned size) override { return _read ? _read(buff, size) : 0; }
	protected:
		CBOpen _open;
		CBClose _close;
		CBRead _read;
	};

	template<class CBOpen, class CBClose, class CBWrite>
	class TIOCbWrite : public IO {
	public:
		TIOCbWrite(CBOpen cbopen, CBClose cbclose, CBWrite cbwrite) :_open(cbopen), _close(cbclose), _write(cbwrite) {}
	public:///status ctrl
		virtual bool Open(void* para) override { return _open && _open(para) && IO::Open(para); }
		virtual bool Close() override { return IO::Close() && _close && _close(); }
		virtual unsigned Write(void* buff, unsigned size) override { return _write ? _write(buff, size) : 0; }
	protected:
		CBOpen _open;
		CBClose _close;
		CBWrite _write;
	};
}

