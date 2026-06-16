#pragma once
#include "Element.h"

namespace smf{
	class Pipeline 
		:public Object::List<16, Element, Element>
	{
	private:
		using Base = Object::List<16, Element, Element>;
	public:
		Pipeline();
		virtual ~Pipeline();
	protected:
		virtual bool set(uint32_t key, void* val)override;
		virtual bool get(uint32_t key, void* val)const override;
	protected:
		virtual bool statusChangeStep(EStatus)override;
	protected:
		virtual bool processMessage(smf_message_t& msg)override;
		virtual bool processFrame(Frame* ifrm, Frame*& ofrm)override;
		virtual bool generateFrame(Frame*& ofrm)override;
		virtual bool receiveFrame(Frame* ifrm)override;
	protected:
		virtual void beforeLink(Port* port, bool link) override;
	public:
		virtual bool Schedule() override;
		virtual bool run(void*para) override;
	public:
		virtual void Running(bool val) override;
	protected:
		void initSource() const;
		void initSink() const;
		bool linkx(uint8_t sink_idx, uint8_t src_idx);
	public:
		friend class Element;
		friend class Source;
		friend class Sink;
	public:
		enum PLExtFlags {
			EF_AutoDestroyAfterEOS = 1u<<0,
			EF_AutoDestroyAfterDONE = 1u<<1,
			EF_AutoDestroyAfterERROR = 1u<<2,
		};
	protected:
		uint32_t* _pmsg = 0;
		Element* _selected = 0;
	};
}


