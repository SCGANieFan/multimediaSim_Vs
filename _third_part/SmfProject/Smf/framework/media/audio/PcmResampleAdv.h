#pragma once
#include "Object.h"
#include "PcmResample.h"
namespace smf {
	namespace audio {
		class PcmResampleAdv :public ObjectStatic {
		public:	
			~PcmResampleAdv();
		public:
			bool Reset();
			bool Reset(uint32_t rate, uint32_t target_rate
				, uint8_t channels, uint8_t sample_bits, uint8_t sample_width
				, int8_t left_shift, bool is_float);
			bool Run(void* ibuf, int& isize, void* obuf, int& osize);
			operator bool()const;
			bool Valid()const;
		protected:
			void* _hd = 0;
			PcmResample* _res = 0;
		protected:
			bool reset(int rate, int target_rate, int8_t channels, int8_t sample_bits, int8_t sample_width, int8_t left_shift);
			typedef bool (*CBShift)(void* data, int size, int8_t bits, int8_t shf);
			CBShift _shift = 0;
			int8_t _shf = 0;
			int8_t _bits = 0;
			int8_t _width = 0;
		};
		struct smf_resamples_t{
			int (*get_size)(int bits);
			bool (*open)(void* hd, int chs, int targetChs, int rate, int targetRate, int bits, int targetBits);
			bool (*run)(void* hd, const void* inbuf, int* insize, void* outbuf, int* outsize);
			void (*close)(void* hd);
		};
	}
}