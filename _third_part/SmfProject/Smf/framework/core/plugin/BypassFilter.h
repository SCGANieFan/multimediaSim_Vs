#pragma once
#include "Filter.h"
namespace smf {
	class BypassFilter
		:public Filter
	{
	public:
		BypassFilter();
		virtual ~BypassFilter();
	public:
		virtual bool Schedule() override;
	protected:
		virtual bool processFrame(Frame* ifrm, Frame*& ofrm) override;
		virtual bool set(uint32_t key, void* val) override;
	protected:
		uint32_t _timestamp = 0;
		uint32_t _isizePerSecond = 0;
		// mutex _mtx;
		mutex* _pmtx = 0;//&_mtx;
	};

	class Selector : public BypassFilter {
	private:
		Ports _iports{ this, true };
		Ports _oports{ this, false };
		uint8_t _iportIdx = 0;
		uint8_t _oportIdx = 0;
	protected:
		virtual bool open(void*)override;
		//virtual bool close()override;
		virtual bool set(uint32_t key, void* val)override;
	};

	class Collector : public Filter {
	private:
		Ports _iports{ this, true };
	protected:
		virtual bool receiveFrame(Frame* ifrm) override;
		virtual bool generateFrame(Frame*& ofrm) override;
	};

	class Divider : public Filter {
	private:
		Ports _oports{ this, false };
	protected:
		virtual bool receiveFrame(Frame* ifrm) override;
		virtual bool generateFrame(Frame*& ofrm) override;
	};
}
