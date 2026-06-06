#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#if 0
#include "minimp4.h"

#define LOG(fmt,...) printf("%s/%d#%s()" fmt "\n", strrchr(__FILE__,'\\') + 1, __LINE__, __func__,  ##__VA_ARGS__)
#define PATH "../../source/video/mp4/"
#define FILE_NAME "xing1000.mp4"



struct Box32 {
	uint32_t _size;
	uint32_t _name;
};
struct Box32Full :public Box32 {
	uint32_t _version;
};
struct BoxPattern {
	uint32_t size;
	uint32_t name;
	uint32_t paras[32];
};


struct Box_moof : public Box32 {
	struct Box_mfhd :public Box32Full {
		uint32_t _sequence_number;
	}mfhd;
	struct Box_traf :public Box32 {
		struct Box_tfhd :public Box32Full {
			uint32_t _track_id;
		}tfhd;
		struct Box_trun :public Box32Full {
			uint32_t _sample_count;
			int32_t _data_offset;
			uint32_t _duration;
			uint32_t _data_bytes;
		}trun;
	}traf;
};

struct Moof : Box32 {//Movie Fragment Box32
	struct Mfhd : Box32 {//Movie Fragment Header Box32
		uint32_t seqNO;
	}mfhd;
	struct Traf : Box32 {//Track Fragment Box32
		struct tfhd :Box32Full {//Track Fragment Header Box32
			uint32_t track_id;
			// all the following are optional fields 
			uint64_t base_data_offset;//0x000001 
			uint32_t sample_description_index;//0x000002 
			uint32_t default_sample_duration;//0x000008 
			uint32_t default_sample_size;//0x000010 
			uint32_t default_sample_flags;//0x000020 
		}trhd;
		struct Trun : Box32Full {
			uint32_t sample_count;
			// the following are optional fields
			int32_t data_offset;
			uint32_t first_sample_flags;//0x000004 
			struct Item {
				uint32_t sample_duration;//0x000100 
				uint32_t sample_size;//0x000200 
				uint32_t sample_flags;//0x000400 
				union {//0x000800 
					uint32_t sample_composition_time_offset_u;//version == 0
					int32_t sample_composition_time_offset_s;
				};
			}items[1];//[sample_count];
		}trun;
	}traf;
};

template<int __vfcount, int __afcount>
struct Moof_AV : Box32 {//Movie Fragment Box32
	struct Mfhd : Box32 {//Movie Fragment Header Box32
		uint32_t seqNO;
	}mfhd;
	struct Traf_v : Box32 {//Track Fragment Box32
		struct Tfhd :Box32Full {//Track Fragment Header Box32
			uint32_t track_id;
			// all the following are optional fields 
			//uint64_t base_data_offset;//0x000001 
			uint32_t sample_description_index;//0x000002 
			uint32_t default_sample_duration;//0x000008 
			uint32_t default_sample_flags;//0x000020 
		}tfhd;
		struct Trun : Box32Full {
			uint32_t sample_count;
			// the following are optional fields
			int32_t data_offset;//0x000001 
			uint32_t first_sample_flags;//0x000004 
			struct Item {
				uint32_t sample_size;//0x000200 
				uint32_t sample_composition_time_offset;//0x000800
			}items[__vfcount];//[sample_count];
		}trun;
	}video;
	struct Traf_a : Box32 {//Track Fragment Box32
		struct tfhd :Box32Full {//Track Fragment Header Box32
			uint32_t track_id;
			// all the following are optional fields 
			//uint64_t base_data_offset;//0x000001 
			uint32_t sample_description_index;//0x000002 
			uint32_t default_sample_duration;//0x000008 
			uint32_t default_sample_flags;//0x000020 
		}tfhd;
		struct Trun : Box32Full {
			uint32_t sample_count;
			// the following are optional fields
			int32_t data_offset;//0x000001 
			// all fields in the following array are optional
			struct Item {
				uint32_t sample_size;//0x000200 
				uint32_t sample_composition_time_offset;//0x000800
			}items[__afcount];//[sample_count];
		}trun;
	}audio;
};



constexpr inline uint32_t Swap32(const uint32_t v) {
	return (((v) << 24) | ((v) >> 24) | ((v) >> 8 << 24 >> 8) | ((v) << 8 >> 24 << 8));
}

struct FMp4Parser_t {
	MP4D_demux_t* _mp4 = 0;
	int64_t _moofPosition = 0;
	uint32_t _moofIndex = 0;
	void* _tempBuff = 0;
	uint32_t ret_continue;
};

struct Mp4TrackInfo {
	char* codec;
	int _trIdx = -1;
	int _portIdx = -1;
	uint8_t* _extraData = 0;
	uint32_t _extraSize = 0;
	uint32_t _currIdx = 0;
	int _frameOffset = 0;
	bool _isDeleteIO = false;
	bool _first = true;
	uint64_t _mediaType = 0;
	uint32_t _frameCount = 0;
	uint32_t _position = 0;
	//uint32_t _startTs = 0;
	uint32_t _timescale = 0;
	uint32_t _duration = 0;
	uint32_t _timestamp = 0;
	//int64_t _timestamp_delta = 0;
	MP4D_demux_t* _mp4 = 0;
	FMp4Parser_t* _fmp4 = 0;
	bool (*_cbRead)(Mp4TrackInfo *info, void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration) = 0;
};

static char filenames[][50] = {
	"xing1000.mp4"
};

static FILE* fpRead = 0;
static Mp4TrackInfo trackInfo_g[2];
static FMp4Parser_t fMp4Parser_g;

static int Mp4FileReadCallback(int64_t offset, void* buffer, size_t size, void* token) {
	FILE* fp = (FILE*)token;
	if (fp != fpRead) {
		LOG("%p!=%p", fp, fpRead);
		return -1;
	}
	fseek(fp, offset, SEEK_SET);
	int rsize = fread(buffer, 1, size, fp);
	return rsize;
}

static bool readX(FMp4Parser_t* fMp4Parser, void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration, uint32_t& track_id) {
	fseek(fpRead, fMp4Parser->_moofPosition, SEEK_SET);
	auto xsize = sizeof(Box_moof);
	Box_moof moofX;
	auto rsize = fread(&moofX, 1, xsize, fpRead);
	if (rsize != xsize) {
		LOG("rsize != xsize");
		return false;
	}
	auto moof = (Box_moof*)&moofX;
#if 0
	returnIfErrE(false, moof->_name != fcc32("moof"), SMF_ERROR_IO_EOS);
	returnIfErrC(false, moof->_size != Swap32(sizeof(Box_moof)));
#endif
	fMp4Parser->_moofIndex++;
	//read mdat
	xsize = 4 * 2;
	rsize = fread(&moofX, 1, xsize, fpRead);
	if (rsize != xsize) {
		LOG("rsize != xsize");
		return false;
	}
	auto mdat = (Box32*)&moofX;
#if 0
	//returnIfErrC(false, mdat->_name != fcc32("mdat"));
#endif
	xsize = Swap32(mdat->_size) - 8;
	if (xsize != Swap32(moof->traf.trun._data_bytes)) {
		LOG("");
		return false;
	}
	fMp4Parser->_moofPosition += sizeof(Box_moof) + 8 + xsize;
	//
	if (track_id && Swap32(moof->traf.tfhd._track_id) != track_id) {
		LOG("");
		fMp4Parser->ret_continue = true;
		return true;
	}
	//
	rsize = fread(data, 1, xsize, fpRead);
	if (rsize != xsize) {
		LOG("");
		return false;
	}
	size = rsize;
	duration = Swap32(moof->traf.trun._duration);
	track_id = Swap32(moof->traf.tfhd._track_id);
	return true;
}

static bool readFragment(Mp4TrackInfo* info, void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration) {
	uint32_t track_id = info->_trIdx + 1;
	FMp4Parser_t* fMp4Parser = info->_fmp4;
	fMp4Parser->ret_continue = false;
	while (1) {
		bool ret = readX(fMp4Parser, data, size, timestamp, duration, track_id);
		if(!ret) return false;
		if (!fMp4Parser->ret_continue) {
			break;
		}
		fMp4Parser->ret_continue = false;
	}
	return true;
}

static bool readNormal(Mp4TrackInfo* info, void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration) {
	auto frameindex = info->_currIdx;
	auto frame_bytes = 0u; 
	MP4D_file_offset_t ofs = MP4D_frame_offset(info->_mp4, info->_trIdx, frameindex, &frame_bytes, &timestamp, &duration);
	if (!ofs) {
		LOG("!ofs");
		return false;
	}
	unsigned left = size;
	//uint32_t timestamp_ms = (uint64_t)_timestamp * 1000u / _timescale;
	if (info->_frameOffset || (frame_bytes > left)) {
		frame_bytes -= info->_frameOffset;
		ofs += info->_frameOffset;
		if (frame_bytes > left) {
			frame_bytes = left;
			info->_frameOffset += left;
		}
		else {
			info->_frameOffset = 0;
		}
	}
	fseek(fpRead, ofs, SEEK_SET);
	int rsize = fread(data, 1, frame_bytes, fpRead);
	//int rsize = _io->Read(data, frame_bytes);
	if (!rsize) {
		LOG("!rsize");
		return false;
	}
	if (rsize != frame_bytes) {
		LOG("rsize != frame_bytes");
		return false;
	}
	size = rsize;
	info->_position = ofs;
	return true;
}

static void Mp4Test0() {
	MP4D_demux_t mp4;
	fpRead = fopen(PATH FILE_NAME, "rb");
	if (!fpRead) {
		LOG("!fpRead");
		return;
	}
	fseek(fpRead,0,SEEK_END);
	int64_t file_size = ftell(fpRead);
	fseek(fpRead, 0, SEEK_SET);
	
	MP4D_open(&mp4, Mp4FileReadCallback, fpRead, file_size);

	if (!mp4.track_count) {
		LOG("!mp4.track_count");
		return;
	}
	FMp4Parser_t& fMp4Parser = fMp4Parser_g;
	if (mp4.moof_start_pos) {
		fMp4Parser._mp4 = &mp4;
		fMp4Parser._moofPosition = mp4.moof_start_pos;
		fMp4Parser._tempBuff = 0;
		fMp4Parser._moofIndex = 1;
		int64_t _moofPosition = mp4.moof_start_pos;
		uint32_t _moofIndex = 1;
	}
	for (int i = 0; i < mp4.track_count; ++i) {
		Mp4TrackInfo* trackInfo = &trackInfo_g[i];
		char* &codec = trackInfo->codec;
#if 1
		switch (mp4.track[i].object_type_indication) {
		case 0x00: codec = 0; break;//"Forbidden";
		case 0x01: codec = 0; break;//"Systems ISO/IEC 14496-1";
		case 0x02: codec = 0; break;//"Systems ISO/IEC 14496-1";
		case 0x20: codec = 0; break;//"Visual ISO/IEC 14496-2";
		case 0x21: codec = (char*)"h264"; break;
		case 0x22: codec = (char*)"h264"; break;
		case 0x23: codec = (char*)"h265"; break;
		case 0x40: codec = (char*)"aac"; break;
		case 0x60: codec = 0; break;//"Visual ISO/IEC 13818-2 Simple Profile";
		case 0x61: codec = 0; break;//"Visual ISO/IEC 13818-2 Main Profile";
		case 0x62: codec = 0; break;//"Visual ISO/IEC 13818-2 SNR Profile";
		case 0x63: codec = 0; break;//"Visual ISO/IEC 13818-2 Spatial Profile";
		case 0x64: codec = 0; break;//"Visual ISO/IEC 13818-2 High Profile";
		case 0x65: codec = 0; break;//"Visual ISO/IEC 13818-2 422 Profile";
		case 0x66: codec = (char*)"aac"; break;//"Audio ISO/IEC 13818-7 Main Profile";
		case 0x67: codec = (char*)"aac"; break;//"Audio ISO/IEC 13818-7 LC Profile";
		case 0x68: codec = (char*)"aac"; break;//"Audio ISO/IEC 13818-7 SSR Profile";
		case 0x69: codec = (char*)"mp3"; break;//"Audio ISO/IEC 13818-3";
		case 0x6A: codec = 0; break;//"Visual ISO/IEC 11172-2";
		case 0x6B: codec = (char*)"aac"; break;//"Audio ISO/IEC 11172-3";
		case 0x6C: codec = (char*)"jpeg"; break;//"Visual ISO/IEC 10918-1";
		//case fcc32("alac"):return fcc32("alac");
		default:
			codec = 0;
			//if (v >= 0xC0)return fcc32("user");// http://www.mp4ra.org/object.html 0xC0-E0  && 0xE2 - 0xFE are specified as "user private"
			break;
		}
#endif
		MP4D_track_t* track = &mp4.track[i];
		LOG("0x%x -> %s, stream_type:0x%x", track->object_type_indication, codec? codec:" ", track->stream_type);
		switch (track->stream_type) {
		case 0x04: {// VisualStream
			LOG("wxh=%dx%d", track->SampleDescription.video.width, track->SampleDescription.video.height);
			LOG("timescale:%d,duration_lo:%d,sample_count:%d", track->timescale, track->duration_lo, track->sample_count);
			if (track->timescale) {
				auto frameRate = track->duration_lo ? (track->sample_count * track->timescale / track->duration_lo) : 20;
				LOG("frameRate:%d", frameRate);
			}
			break;
		}
		case 0x05: {// AudioStream
			auto rate = track->SampleDescription.audio.samplerate_hz;
			if (!rate) { rate = track->timescale; }
			auto channels = track->SampleDescription.audio.channelcount;
			LOG("rate:%d,timescale:%d,channel:%d,",
				track->SampleDescription.audio.samplerate_hz, 
				track->timescale, 
				track->SampleDescription.audio.channelcount);
			break;
		}
		case 0xC0://OtherStream
			LOG("OtherStream");
			break;
		}

		trackInfo->_trIdx = i;
		trackInfo->_timescale = track->timescale;
		trackInfo->_duration = trackInfo->_timescale ? track->duration_lo * 1000ll / trackInfo->_timescale : 0;
		trackInfo->_extraData = track->dsi;
		trackInfo->_extraSize = track->dsi_bytes;
		trackInfo->_frameCount = track->sample_count;
		trackInfo->_mp4 = &mp4;
		trackInfo->_fmp4= &fMp4Parser;
		LOG("timescale:%d,duration:%d,extraData:%p,extraSize:%d,frameCount:%d",
			trackInfo->_timescale,
			trackInfo->_duration,
			trackInfo->_extraData,
			trackInfo->_extraSize,
			trackInfo->_frameCount
			);

		if (mp4.moof_start_pos) {
			trackInfo->_cbRead = &readFragment;
			//_frameCount = 0xffffffff;
		}
		else {
			trackInfo->_cbRead = &readNormal;
		}
	}
	LOG("title,%p,%d",mp4.tag.title,mp4.tag.title_size);
	LOG("artist,%p,%d",mp4.tag.artist,mp4.tag.artist_size);
	LOG("album,%p,%d",mp4.tag.album,mp4.tag.album_size);


	uint32_t frameCountMax = 0;
	for (int i = 0; i < mp4.track_count; ++i) {
		frameCountMax = frameCountMax < trackInfo_g[i]._frameCount ? trackInfo_g[i]._frameCount : frameCountMax;
	}
	for (uint32_t f = 0; f < frameCountMax; f++) {
		for (int i = 0; i < mp4.track_count; ++i) {
			Mp4TrackInfo* trackInfo = &trackInfo_g[i];
			if (f > trackInfo->_frameCount) continue;
			unsigned timestamp = 0;
			unsigned duration = 0;
			unsigned frame_bytes = 0;
			const uint32_t readBufByte = 100 * 1024;
			static uint8_t readBuf[readBufByte];
			uint32_t readBufByteActual = readBufByte;
			trackInfo->_cbRead(trackInfo, readBuf, readBufByteActual, timestamp, duration);
			//LOG("[%d,%s][%d]%p,%d", i, trackInfo->codec, f, readBuf, readBufByteActual);

			if (!timestamp && duration) {
				timestamp = trackInfo->_timestamp;
				trackInfo->_timestamp += duration;
			}
			else {
				trackInfo->_timestamp = timestamp;
			}
			uint32_t timestamp_ms = (uint64_t)timestamp * 1000u / trackInfo->_timescale;
			if (trackInfo->_first) {
				trackInfo->_first = false;
			}
			if (trackInfo->_frameOffset) {
			}
			else {
				trackInfo->_currIdx++;
			}
		}
	}

	fclose(fpRead);
}



class BoxHead_c
{
public:
	BoxHead_c() {}
	~BoxHead_c() {}
public:
	void Prase(uint8_t *data, uint32_t dataLen) {
		size = (((uint32_t)data[0]) << 24)
			| (((uint32_t)data[1]) << 16)
			| (((uint32_t)data[2]) << 8)
			| (((uint32_t)data[3]) << 0);
		memcpy(type, data + 4, 4);
		type[4] = 0;
	}
public:
	uint32_t size = 0;
	uint8_t type[5];
};




static void Mp4Test1() {
	fpRead = fopen(PATH FILE_NAME, "rb");
	if (!fpRead) {
		LOG("!fpRead");
		return;
	}
	fseek(fpRead, 0, SEEK_END);
	int64_t file_size = ftell(fpRead);
	fseek(fpRead, 0, SEEK_SET);

	BoxHead_c boxHead;
	uint32_t cnt = 1;
	int64_t boxSize = 0;
	while (1) {
		uint8_t data[8];
		uint32_t readByte = fread(data, 1, sizeof(data), fpRead);
		if (readByte != sizeof(data))
			break;
		boxHead.Prase(data, sizeof(data));
		LOG("[%d] %s, %u", cnt, (char*)boxHead.type, boxHead.size);
#if 0
		if (boxHead.size >= 2 && boxHead.size < 8) {
			LOG("invalid box size (broken file?)");
		}
		if (boxHead.size == 0 ||                         // standard indication of 'till eof' size
			boxHead.size == (boxsize_t)0xFFFFFFFFU       // some files uses non-standard 'till eof' signaling
			)
		{
			boxHead.size = ~(boxsize_t)0;
		}
#endif
		++cnt;
		if (boxHead.size < 8) {
			LOG("boxHead.size < 8");
			continue;
		}
		boxSize += boxHead.size;
		fseek(fpRead, boxHead.size - 8, SEEK_CUR);
	}

	LOG("%lld,%lld", file_size, boxSize);
	fclose(fpRead);
}


void Mp4Test()
{
	LOG();
	Mp4Test0();
	//Mp4Test1();
}
#endif

#include "mp4_api.h"

#define LOG(fmt,...) printf("%s/%d#%s()" fmt "\n", strrchr(__FILE__,'\\') + 1, __LINE__, __func__,  ##__VA_ARGS__)

#define PATH "../../source/video/mp4/"
#define FILE_NAME "xing1000.mp4"


static void* Mp4Malloc(uint32_t size) {
	static int32_t sizeTotal = 0;
	sizeTotal += size;
	void* ptr = malloc(size);
	LOG("malloc, ptr:%p, size:%d, sizeTotal:%d,", ptr, size, sizeTotal);
	return ptr;
}

static void* Mp4Realloc(void* ptr, uint32_t size) {
	void* ptrNew = realloc(ptr, size);
	LOG("realloc, (%p->%p,%d)", ptr, ptrNew, size);
	return ptrNew;
}

static void Mp4Free(void* ptr) {
#if 1
	LOG("free, ptr:%p", ptr);
#endif
	return free(ptr);
}
static void Mp4Print(const char* buf, uint32_t len) {
	printf("%s", buf);
}

#if 0
static int32_t Mp4FileReadCallback(uint32_t offset, void* buffer, uint32_t size, void* priv) {
	FILE* fp = (FILE*)priv;
	if(!fp) return -1;
	fseek(fp, offset, SEEK_SET);
	int rsize = fread(buffer, 1, size, fp);
	return rsize;
}
#endif

static int32_t Mp4FileSeekCallback(uint32_t offset, void* priv) {
	FILE* fp = (FILE*)priv;
	if (!fp) return 0;
	return fseek(fp, offset, SEEK_SET);
}

static int32_t Mp4FileReadCallback(void* buffer, uint32_t size, void* priv) {
	FILE* fp = (FILE*)priv;
	if (!fp) return 0;
	return fread(buffer, 1, size, fp);
}

static void Mp4Test0() {
	Mp4ApiBasePort_t bp;
	memset(&bp, 0, sizeof(Mp4ApiBasePort_t));
	bp.malloc_cb = Mp4Malloc;
	bp.realloc_cb = Mp4Realloc;
	bp.free_cb = Mp4Free;
	bp.printf_cb = Mp4Print;
	void* hd = mp4_api_demuxer_create(&bp);
	//if (!hd) return;
	Mp4ApiRet_t ret = MP4_API_RET_FAIL;
	
	FILE *fpRead = fopen(PATH FILE_NAME, "rb");
	if (!fpRead) {
		LOG("!fpRead");
		return;
	}
	fseek(fpRead, 0, SEEK_END);
	uint32_t file_size = (uint32_t)ftell(fpRead);
	fseek(fpRead, 0, SEEK_SET);
	ret |= mp4_api_demuxer_set(hd, "file_priv", (void*)(fpRead));
	ret |= mp4_api_demuxer_set(hd, "file_size", (void*)file_size);
	ret |= mp4_api_demuxer_set(hd, "file_read_cb", (void*)(Mp4FileReadCallback));
	ret |= mp4_api_demuxer_set(hd, "file_seek_cb", (void*)(Mp4FileSeekCallback));

	ret = mp4_api_demuxer_open(hd);
	
	const uint32_t bufTmpByte = 100 * 1024;
	static uint8_t bufTmp[bufTmpByte];
	while(1) {
		uint8_t* pBuf = bufTmp;
		int32_t bufByte = bufTmpByte;
		ret = mp4_api_demuxer_generate(hd, pBuf, &bufByte);
		if (ret == MP4_API_RET_FAIL) {
			break;
		}
	}
	
	ret = mp4_api_demuxer_close(hd);
	ret = mp4_api_demuxer_destory(hd);

}

void Mp4Test()
{
	LOG();
	Mp4Test0();
}

