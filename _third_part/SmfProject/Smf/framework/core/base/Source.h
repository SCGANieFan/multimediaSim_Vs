#pragma once
#include "Element.h"
namespace smf{	
	class Source
		:public Element
	{
	public:
		static constexpr const char* __type = "src";
	public:
		Source();
		//virtual ~Source();
	private:
		Port _portOut{ this,false };
	protected:
		uint32_t _frameIndex = 0;
		mutable SharedInfo_t _sinfo;
		struct volume_t{
			uint16_t volume;
			bool mute;
			uint8_t flags;
		};
		mutable volume_t _volume;
	public://pipeline
		virtual bool Schedule() override;
		virtual SharedInfo_t* SharedInfo()const override;
	protected:
		virtual bool open(void*)override;
		virtual bool run(void*) override;
		virtual bool set(uint32_t key, void* val)override;
		virtual bool get(uint32_t key, void* val)const override;
		virtual bool processFrame(Frame* ifrm, Frame*& ofrm) override final { return false; }
		//virtual bool generateFrame(Frame*& ofrm) override final { return false; }
		virtual bool receiveFrame(Frame* ifrm) override final { return false; }
	};
}
