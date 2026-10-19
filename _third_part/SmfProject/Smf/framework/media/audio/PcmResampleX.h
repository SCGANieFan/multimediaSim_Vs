#pragma once
#include "Object.h"

namespace smf {
	namespace audio {
		class PcmResampleX :public ObjectStatic {
		public:
			PcmResampleX();
			~PcmResampleX();
		public:
			typedef void (*CbResample)(void* ibuf, int& isize, void* obuf, int& osize, PcmResampleX*);
			bool Reset();
			bool Reset(int rate, int channels, int sample_bits, int target_rate = 0);
			bool Run(void* ibuf, int& isize, void* obuf, int& osize);
			bool OutputRateAdjust(int adjust);
			void RateUpdate(uint32_t irate, uint32_t orate);
			CbResample GetResample()const;
			operator bool()const;
			bool Valid()const;
		protected:
			CbResample _cbResample = 0;
			static const CbResample _cbResamples[3][4];
		public:
			uint64_t _irate = 0;
			uint64_t _orate = 0;
			uint8_t _bits = 0;
			uint8_t _channels = 0;
			//
			int32_t _oadjust = 0;
		public:
			uint64_t _ipos = 0;
			uint64_t _opos = 0;
			int32_t _vs[16][2];
		};
	}
}