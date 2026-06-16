#pragma once
#include "smf_common.h"
#include "IMedia.h"
#include "IHook.h"
#include "Frame.h"
#include "ObjectStatic.h"

namespace smf {
	class Element;
	class Filter;
	class Frame;
	class Port
		: public ObjectStatic
	{
	public:
		friend class Ports;
	public:
		Port(Element* parent, bool isInput);
		virtual ~Port();
	protected:
		Port* _linker = 0;
		Element* _parent = 0;
	public:
		union {
			uint32_t _flags = 0;
			struct {
				bool _isInport : 1;
				bool _deleteMedia : 1;
				bool _deleteHook : 1;
				bool _deleteFrame : 1;				
				bool _isMultiLink : 1;
				bool _isEos : 1;
				bool _mediaUpdate : 1;
				bool _isPushNoWait : 1;
				
				bool _noAllocFrame : 1;
				bool _noAllocFrameBuff : 1;
				bool _isAudio : 1;
				bool _isVideo : 1;
				bool _isOther : 1;
				bool _isPushSleep : 1;
				bool _enLog : 1;
				bool _enLogTimeStamp : 1;

				bool _enLogMedia : 1;
				bool _reallocFrame : 1;
				bool _disableSendEOSMsg : 1;
				bool _dislinkWait : 1;
				bool _dislinkWaiting : 1;
				bool _processing : 1;
				uint8_t _rev5 : 2;

				uint8_t _rev8 : 8;
			};
		};
	public:
		IHook* _hook = 0;
		Frame* _frame = 0;
		IMedia* _media = 0;
		uint32_t _max = 0;
		uint32_t _min = 0;
	protected:
		uint8_t _count = 0;
		uint8_t _index = 0;
	protected:
		mutable uint16_t _countFrame = 0;
		mutable uint16_t _countPost = 0;
		mutable uint16_t _countLoop = 0;
	public:
		Element* Parent()const { return _parent; }
		bool IsInPort()const { return _isInport; }
		bool IsMultiLink()const { return _isMultiLink; }
		bool IsPushNoWait()const { return _isPushNoWait; }
		void IsPushNoWait(bool v) { _isPushNoWait = v; }
		uint64_t CodecType()const;
		void SetReallocFrame(uint32_t max);
	public:
		void Print(Frame*) const;
		char* Print(char* ptr, char* end, Frame*) const;
		char* Print(char* ptr, char* end) const;
	protected:
		void dbgErrProcess(const char* file, unsigned line, unsigned error)const;
	public:	
		virtual bool Link(Port*);
		virtual bool DisLink(Port*);
		virtual void DisLink();
		virtual bool Replace(Port*);
		virtual bool Push(smf_message_t&);
	public:
		Port* Linker() const { return  (Port*)_linker; }
		Port*& Linker() { return  (Port*&)_linker; }
		Element* LinkedElement() const {return _linker ? _linker->_parent : 0;}
	public:
		void Set(IMedia* media, bool deleteMedia = false);
		void Set(Frame* frame, bool deleteFrame = false) { _frame = frame; _deleteFrame = deleteFrame; }
		bool Set(const char* codec);
		bool SetMediaScript(const char* script);
		bool SetAndClone(IMedia*);
		bool Set(IHook* hook, bool deleteHook);
		IMedia* Media() const;
	public:
		uint32_t MinSize()const { return _min; }
		uint32_t MaxSize()const { return _max; }
	public:
		Frame* InitFrame(Frame* frm = 0);
		bool PostFrame(Frame* frm, Element* ele);
	public:
		bool GetX(uint32_t key, void* val)const;
		bool SetX(uint32_t key, void* val);
		bool Pull(Frame*& frm) { return Pull(frm, _linker); }
		bool Push(Frame* frm) { return Push(frm, _linker); }
	public:
		bool Push(Frame* frm, Port* port);
		bool Pull(Frame*& frm, Port* port);
	protected:
		struct RecordProcess {
			Port* _port;
			RecordProcess(Port* port):_port(port) {port->_processing = true;}
			~RecordProcess(){ _port->_processing = false; }
		};
		void dislinkWait();
	};

	class Ports
		: public Port
	{
	public:
		Ports(Element* parent, bool isInput);
		virtual ~Ports();
	private:
		Port* _linkers[8];
	public:
		int Max()const { return 8; }
		int Count()const { return _count; }
		int CountGet()const;
		bool SetMultiLink(Port** linkers, int max);
	public:
		virtual bool Link(Port*)override;
		virtual bool DisLink(Port*)override;
		virtual void DisLink()override;
		virtual bool Replace(Port*)override;
		virtual bool Push(smf_message_t&)override;
		using Port::Push;
		using Port::Pull;
	public:
		Port* Linker(int idx) const { return  (Port*)_linkers[idx]; }
		Element* LinkedElement(int idx) const {	return _linkers[idx] ? _linkers[idx]->Parent() : 0;	}
		int Index(Port*)const;
		Port* Select(int idx) { return _linker = _linkers[idx]; }
	public:
		bool Pull(Frame*& frm, int idx) { return Pull(frm, _linkers[idx]); }
		bool Push(Frame* frm, int idx) { return Push(frm, _linkers[idx]); }
		bool PushAll(Frame* frm);
	};
}

