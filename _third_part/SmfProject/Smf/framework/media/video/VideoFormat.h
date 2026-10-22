#pragma once
#include "Object.h"

namespace smf {
	namespace video {
		typedef enum {
			CENTER,
			TOP_RIGHT,
			BOTTOM_RIGHT,
			TOP_LEFT,
			BOTTOM_LEFT
		} align_e;
		class VideoFormat: public ObjectStatic {
		public:
			~VideoFormat();
			friend class VideoFormatList;
		public:
			bool Reset();
			bool Reset(
				uint64_t icodec, uint8_t ifmt, uint16_t iw, uint16_t ih,uint16_t ix, uint16_t iy,uint16_t ixsize,uint16_t iysize
				, uint64_t ocodec, uint8_t ofmt, uint16_t ow, uint16_t oh, uint16_t ox, uint16_t oy, uint16_t oxsize, uint16_t oysize
				, bool scale = false, bool crop = false
			);
			bool Format(void* idata, void*& odata);
			bool adaptiveDispResolution();
			uint32_t InputSize()const { return _isize; }
			uint32_t OutputSize()const { return _osize; }
		public:
			uint16_t _iw = 0;
			uint16_t _ih = 0;
			uint16_t _ix = 0;
			uint16_t _iy = 0;
			uint16_t _ixsize = 0;
			uint16_t _iysize = 0;
			uint64_t _icodec = 0;
			uint32_t _isize = 0;
			uint8_t _ifmt = 0;
		public:
			uint16_t _ow = 0;
			uint16_t _oh = 0;
			uint16_t _ox = 0;
			uint16_t _oy = 0;
			uint16_t _oxsize = 0;
			uint16_t _oysize = 0;
			uint64_t _ocodec = 0;
			uint32_t _osize = 0;
			uint8_t _ofmt = 0;
		public:
			bool _scale = false;
			bool _crop = false;
			bool _mirror = false;
			bool _rotate_180 = false;
			align_e _align = CENTER;
		protected:
			typedef bool (*Func)(VideoFormat* vf, void* idata, void*& odata);
			Func _func = 0;
		public:
			void* _priv = 0;
		};
	}
}