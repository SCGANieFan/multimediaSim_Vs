#pragma once
#include "VideoFormat.h"
#include "MediaVideo.h"
#include "SmfHash.h"
namespace smf {
	namespace video {
		class VideoFormatList : public ObjectStatic {
		public:
			VideoFormatList();
		public:
			enum { __copy = 0xffffffff };
			using Func = VideoFormat::Func;
			struct item_t{
				uint32_t _id;
				Func _func;
			};
		protected:
			enum { MAX_ITEMS = 32 };
			item_t _items[MAX_ITEMS];
			uint32_t _cnt = 0;
		public:
			static bool Copy(VideoFormat* vf, void* idata, void*& odata);
		public:
			void Add(uint32_t id, Func func);
			Func Find(uint32_t id);
			Func Find(VideoFormat*);
		};
		static constexpr uint32_t vfid32(const char* icodec, uint32_t ifmt, const char* ocodec, uint32_t ofmt, bool scale=false, bool crop=false) {
			return ((Hash(icodec) != Hash(ocodec)) || (ifmt != ofmt) || scale || crop) ? Hash(Hash(Hash((ifmt << 16) | ofmt, icodec), ocodec), scale|crop) : VideoFormatList::__copy;
		}
	}
}