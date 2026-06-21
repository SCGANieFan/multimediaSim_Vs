#pragma once
#include "Element.h"
namespace smf{
	class Sink 
		:public Element
	{
	public:
		static constexpr const char* __type = "sink";
	public:
		Sink();
		//virtual ~Sink();
	private:
		Port _portIn{ this,true };
	public://pipeline
		virtual bool Schedule() override;
	protected:
		virtual bool run(void*) override;
	protected:
		virtual bool processFrame(Frame* ifrm, Frame*& ofrm) override final { return false; }
		virtual bool generateFrame(Frame*& ofrm) override final { return false; }
		//virtual bool receiveFrame(Frame* ifrm) override final { return false; }
	};
}
