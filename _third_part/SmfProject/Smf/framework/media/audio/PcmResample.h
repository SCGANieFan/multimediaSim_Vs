#pragma once
#include "Object.h"

namespace smf {
	namespace audio {
		class PcmResample :public ObjectStatic {
		//public:	
		//	PcmResample();
		//	~PcmResample();
		public:
			typedef bool (*CbResample)(void* ibuf, int& isize, void* obuf, int& osize, PcmResample&);
			bool Reset();
			bool Reset(uint32_t rate, uint32_t target_rate, uint8_t channels, uint8_t sample_bits, uint8_t sample_width, int8_t left_shift = 0, bool is_float = false);
			bool Run(void* ibuf, int& isize, void* obuf, int& osize);
			operator bool()const;
			bool Valid()const;
		protected:
			CbResample _cbResample = 0;
		public:
			uint32_t _rate = 0;
			uint32_t _target_rate = 0;
			uint8_t _bits = 0;
			uint8_t _width = 0;
			uint8_t _channels = 0;
			int8_t _shift = 0;
		public:
			uint32_t _iidx = 0;
			uint32_t _oidx = 0;
			//uint32_t _cnt = 0;
		public:
			int32_t _refs[32];// max 8 channels × 4 byte/samples,not support double
		};
	}
}