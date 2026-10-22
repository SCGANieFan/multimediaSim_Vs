#pragma once
#include "MediaAudio.h"
#include "SmfFCC.h"
namespace smf {
	class MediaADPcm :public MediaAudio {
	public:
		MediaADPcm();
	protected:
		//virtual bool setX(uint32_t keys, void* vals)override;
		//virtual bool getX(uint32_t keys, void* vals) const override;
	};
}