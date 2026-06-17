#pragma once
#include "Element.h"
namespace smf {
	class Filter
		: public Element
	{
	public:
		static constexpr const char* __type = "filter";
	public:
		Filter();
		//virtual ~Filter();
	private:
		Port _portIn{ this,true };
		Port _portOut{ this,false };
	public:
		virtual bool Schedule() override;
	protected:
		virtual bool processFrame(Frame* ifrm, Frame*& ofrm) override;
		virtual bool generateFrame(Frame*& ofrm) override;
		virtual bool receiveFrame(Frame* ifrm) override;
	};
}