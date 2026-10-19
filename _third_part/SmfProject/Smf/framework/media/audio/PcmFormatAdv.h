#pragma once
#include "Object.h"
#include "PcmFormat.h"
#include "PcmResampleAdv.h"
namespace smf {
	namespace audio {
		class PcmFormatAdv: public ObjectStatic {
		public:
			~PcmFormatAdv() { Reset(); }
		protected:
			PcmFormat _fmt;
			PcmResampleAdv _res;
			void* _buff = 0;
			void* _temp = 0;
			//uint32_t _buffSize = 0;
			uint32_t _buffSizeRatio = 0;
			uint32_t _buffSizeRatioDenum = 0;
		public:
			operator bool()const { return _funcProc!=0; }
			bool Reset();
			bool Reset(uint32_t i_rate, uint32_t o_rate
				, uint8_t i_channels, uint8_t o_channels
				, uint8_t i_bits, uint8_t o_bits
				, uint8_t i_width, uint8_t o_width
				, uint8_t i_float, uint8_t o_float
				, uint8_t i_interlace, uint8_t o_interlace
				, int8_t left_shift = 0
				, bool lrmix = false
				, uint32_t chmap = 0
				, int8_t chNO = -1
			);
			bool Format(void* input, int& insize, void* output, int& outsize, void* tempbuff = 0);
			bool IsCopyOnly()const;
		protected:
			bool(PcmFormatAdv::* _funcProc)(void* idata, int& isize, void* odata, int& osize, void* temp);
			bool processFormat(void* idata, int& isize, void* odata, int& osize, void* temp);
			bool processResample(void* idata, int& isize, void* odata, int& osize, void* temp);
			bool processFormatResample(void* idata, int& isize, void* odata, int& osize, void* temp);
			bool processResampleFormat(void* idata, int& isize, void* odata, int& osize, void* temp);
			bool processCopy(void* idata, int& isize, void* odata, int& osize, void* temp);
		};
	}
}