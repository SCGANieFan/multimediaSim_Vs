#pragma once
#include "ObjectStatic.h"
namespace smf{
namespace audio {
	class PcmGain:public ObjectStatic {
	public:
		~PcmGain();
	public:
		bool gain(void* dst0, void* src0, int32_t scf, int32_t end);
		bool gainAdd(void* dst0, void* src0, int32_t scf, int32_t end);
		bool init(int fsize, int8_t chn, int8_t bits, int8_t w, bool superGain = false);
		void uninit();
	public:
		uint16_t* _facs = 0;
		uint32_t _cnt = 0;
		typedef bool (*CbGain)(PcmGain* pcm, void* dst0, void* src0, int32_t scf, int32_t end);
		CbGain _cbGain = 0;
		CbGain _cbGainAdd = 0;
	};
}
}

