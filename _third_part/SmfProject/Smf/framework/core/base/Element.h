#pragma once
#include "IMedia.h"
#include "Port.h"
#include "Frame.h"
#include "Service.h"
#include "Measure.h"
#include "SmfPorting.h"
#include "IntrLock.h"
#include "mutex.h"

namespace smf{
	class Element
		:public Service
	{
	protected:
		Element();
	public:
		virtual ~Element();
	protected:
		mutable Port* _iport = 0;
		mutable Port* _oport = 0;
	protected://thread
		uint16_t _stackSize = 0;
		uint16_t _processTimeout = 0;
		uint8_t _loopSleepMs = 0;
		uint8_t _threadPriority = 0;
	private:
		bool _threadRunning = false;
		bool _threadLoop = false;
	protected:
		mutable smf_value_t _kips;
		mutable MeasureValue _measureProc;
		smf_pair_t _tempBuff;
	protected:
		using SharedInfo_t = smf_shared_info_t;
		virtual SharedInfo_t* SharedInfo()const;
	public:
		Port* iPort() const { return _iport; }
		IMedia* iMedia() const { return _iport ? _iport->Media() : 0; }
		Port* oPort() const { return _oport; }
		IMedia* oMedia() const { return _oport ? _oport->Media() : 0; }
		bool CanOutput()const { return (bool)_oport; }
		bool CanInput()const { return (bool)_iport; }
		bool IsSource()const { return _flags.Check(IS_Source); }
		bool IsSink()const { return _flags.Check(IS_Sink); }
		bool IsFilter()const { return _flags.Check(IS_Filter); }
		bool IsRunning()const { return _threadRunning; }
		virtual void Running(bool val) { _threadRunning = val; _flags.Set(IS_Loop, val); }
	protected:
		virtual char* print(char* ptr, char* end) const override;
		virtual bool set(uint32_t key, void* val)override;
		virtual bool get(uint32_t key, void* val)const override;
	protected:
		virtual bool start() override;
		virtual bool stop() override;
	protected:
		virtual bool receiveMessage(smf_message_t&) override;
		virtual bool processMessage(smf_message_t&) override;
	public://process
		bool Process(Frame* ifrm, Frame*& ofrm);
		bool Output(Frame*& ofrm);
		bool Input(Frame* ifrm);
		bool Pull(Frame*& ofrm);
		bool Push(Frame* ifrm);
	protected:
		virtual bool processFrame(Frame* ifrm, Frame*& ofrm) = 0;
		virtual bool generateFrame(Frame*& ofrm) = 0;
		virtual bool receiveFrame(Frame* ifrm) = 0;
	protected:
		virtual bool processError(Frame* ifrm, Frame* ofrm);
	protected:
		virtual void beforeLink(Port* port, bool link);
	public://pipeline
		virtual bool Schedule(){return false;}
		bool LinkFrom(Element*);
		bool LinkFrom(const char*);
		bool LinkTo(Element*);
		bool LinkTo(const char*);
		bool DislinkInput();
		bool DislinkOutput();
		using Object::Replace;
		bool Replace(Element* ele);
	public:
		void Kips(uint32_t avg, uint32_t min, uint32_t max);
		void Kips(smf_value_t&val, bool recursive = true) const;
		MeasureValue& Measure() const { return (MeasureValue&)_measureProc; }
		smf_pair_t& TempBuffer() const { return (smf_pair_t&)_tempBuff; }
	private:
		void Loop();
	protected:
		void LoopSleep();
	public:
		friend class Object;
		friend class Port;
		friend class Ports;
		friend class PortIn;
		friend class PortOut;
		friend class Source;
		friend class Sink;
		friend class Filter;
		friend class Pipeline;
		friend class IMedia;
		friend class MeasureCheck;
	};

	class MeasureCheck {
	public:
		MeasureCheck(Element* ele);
		~MeasureCheck();
	protected:
		uint32_t _ms = 0;
		Element* _ele = 0;
	};
}


