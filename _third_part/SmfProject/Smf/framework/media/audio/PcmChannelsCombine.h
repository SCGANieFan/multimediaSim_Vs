#pragma once
#include "Frame.h"
#include "MediaAudio.h"
#include "PcmFormat.h"

namespace smf {
	namespace audio {
		class PcmChannelsCombine: public ObjectStatic {
		protected:
			typedef void (*CbCombine)(void** ifrms, uint8_t* icnts, uint8_t n, void* ofrm, int samples);
			static const CbCombine _cbCombines[4];
		protected:
			CbCombine _cbCombine = 0;
			uint8_t _ichs[8];
			uint16_t _issizes[8];
			uint16_t _ossize = 0;
			uint8_t _ochn = 0;
			uint8_t _n = 0;
			uint8_t _width = 0;
		public:
			operator bool()const { return _cbCombine; }
			bool Valid()const { return _cbCombine; }
			bool Open(uint8_t* chns, uint8_t n, uint8_t width);
			bool Close();
			bool Combine(void** ipcms, void* opcm, int samples);

			bool Open(IMedia** imedias, uint8_t n, IMedia* omedia);
			bool Combine(Frame** ifrms, Frame* ofrm);
		};
	}
}