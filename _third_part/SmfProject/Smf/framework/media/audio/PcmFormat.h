#pragma once
#include "Object.h"

namespace smf {
	namespace audio {
		class PcmFormat: public ObjectStatic {
		protected:
			typedef void (* CbFormatX)(const void* input, void* output, int samples, PcmFormat*);
			CbFormatX _cbFormatX = 0;
			//static const CbFormatX _cbFormatsX[2][2][4][4];
			static const CbFormatX _cbFormats[5][5];
			static const CbFormatX _cbFormatsMix[5][5];
			static const CbFormatX _cbFormatsMap[5][5];
			static const CbFormatX _cbFormatsSelect[5][5];
		public:
			operator bool()const;
			bool Valid()const;
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
			struct PcmSpec {
				uint32_t rate;
				uint8_t  channels;
				uint8_t  bits;
				uint8_t  width;
				bool     is_float;
				bool     interlace;
			};
			bool Reset(const PcmSpec& input, const PcmSpec& output
				, int8_t left_shift = 0
				, bool lrmix = false
				, uint32_t chmap = 0
				, int8_t chNO = -1
			);
			bool Format(void* input, int& insize, void* output, int& outsize);
			
			static bool LeftShift(void* data, int size, int8_t bits, int8_t shf);
		public:
			//media
			uint32_t _i_rate = 0;
			uint8_t _i_channels = 0;
			uint8_t _i_bits = 0;
			uint8_t _i_width = 0;
			bool _i_interlace = false;
			bool _i_float = false;
			//target
			uint32_t _o_rate = 0;
			uint8_t _o_channels = 0;
			uint8_t _o_bits = 0;
			uint8_t _o_width = 0;
			bool _o_interlace = false;
			bool _o_float = false;
			
			//uint8_t _ishift = 0;
			//uint8_t _oshift = 0;
			uint8_t _isampleWidth = 0;
			uint8_t _osampleWidth = 0;
			int8_t _chNO = -1;
			uint32_t _chMap = 0;
		public:
			float _gain = 0.0f;
			int8_t _shift = 0;
			bool _lrmix = false;
			bool _chcopy = true;
		//public://resample
		//	uint64_t _iidx = 0;
		//	uint64_t _oidx = 0;
		//	int32_t _refs[8];
		protected:
			int fmt_size(int& insize, int& outsize);
			int getChsFromMap()const;
		};
	}
}