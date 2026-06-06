#include "mp4_demuxer.h"
#include "minimp4.h"

using namespace GASF_NAME_SPACE;
using namespace mp4_ns;

static constexpr inline uint32_t Swap32(const uint32_t v) {
	return (((v) << 24) | ((v) >> 24) | ((v) >> 8 << 24 >> 8) | ((v) << 8 >> 24 << 8));
}

static int Mp4ReadCallback(int64_t offset, void* buffer, size_t size, void* token) {
	if (!token) return -1;
	Mp4Demuxer_c* demuxer = (Mp4Demuxer_c*)token;
	demuxer->_fileSeekCb((uint32_t)offset, demuxer->_filePriv);
	int rsize = demuxer->_fileReadCb(buffer, (uint32_t)size, demuxer->_filePriv);
	return rsize;
}

static bool readX(Mp4Demuxer_c::FMp4Parser_t* fMp4Parser, void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration, uint32_t& track_id) {
	int32_t xsize = sizeof(Mp4Demuxer_c::Box_moof);
	Mp4Demuxer_c::Box_moof moofX;
	fMp4Parser->_mp4Demuxer_c->_fileSeekCb(fMp4Parser->_moofPosition, fMp4Parser->_mp4Demuxer_c->_filePriv);
	int32_t rsize = fMp4Parser->_mp4Demuxer_c->_fileReadCb(&moofX, xsize, fMp4Parser->_mp4Demuxer_c->_filePriv);
	if (rsize != xsize) {
		//LOG_MP4("rsize != xsize");
		return false;
	}
	auto moof = (Mp4Demuxer_c::Box_moof*)&moofX;
#if 0
	returnIfErrE(false, moof->_name != fcc32("moof"), SMF_ERROR_IO_EOS);
	returnIfErrC(false, moof->_size != Swap32(sizeof(Box_moof)));
#endif
	fMp4Parser->_moofIndex++;
	//read mdat
	xsize = 4 * 2;
	rsize = fMp4Parser->_mp4Demuxer_c->_fileReadCb(&moofX, xsize, fMp4Parser->_mp4Demuxer_c->_filePriv);
	if (rsize != xsize) {
		//LOG_MP4("rsize != xsize");
		return false;
	}
	auto mdat = (Mp4Demuxer_c::Box32*)&moofX;
#if 0
	//returnIfErrC(false, mdat->_name != fcc32("mdat"));
#endif
	xsize = Swap32(mdat->_size) - 8;
	if (xsize != Swap32(moof->traf.trun._data_bytes)) {
		//LOG_MP4("");
		return false;
	}
	fMp4Parser->_moofPosition += sizeof(Mp4Demuxer_c::Box_moof) + 8 + xsize;
	//
	if (track_id && Swap32(moof->traf.tfhd._track_id) != track_id) {
		//LOG_MP4("");
		fMp4Parser->ret_continue = true;
		return true;
	}
	//
	rsize = fMp4Parser->_mp4Demuxer_c->_fileReadCb(data, xsize, fMp4Parser->_mp4Demuxer_c->_filePriv);
	if (rsize != xsize) {
		//LOG_MP4("");
		return false;
	}
	size = rsize;
	duration = Swap32(moof->traf.trun._duration);
	track_id = Swap32(moof->traf.tfhd._track_id);
	return true;
}

bool Mp4Demuxer_c::Mp4TrackInfo_c::ReadFragment(void* data, uint32_t & size, uint32_t & timestamp, uint32_t & duration) {
	uint32_t track_id = _trIdx + 1;
	Mp4Demuxer_c::FMp4Parser_t* fMp4Parser = _fmp4;
	fMp4Parser->ret_continue = false;
	while (1) {
		bool ret = readX(fMp4Parser, data, size, timestamp, duration, track_id);
		if (!ret) return false;
		if (!fMp4Parser->ret_continue) {
			break;
		}
		fMp4Parser->ret_continue = false;
	}
	return true;
}

bool Mp4Demuxer_c::Mp4TrackInfo_c::ReadNormal(void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration) {
	auto frameindex = _currIdx;
	auto frame_bytes = 0u;
	MP4D_file_offset_t ofs = MP4D_frame_offset(_mp4, _trIdx, frameindex, &frame_bytes, &timestamp, &duration);
	if (!ofs) {
		//LOG_MP4("!ofs");
		return false;
	}
	unsigned left = size;
	//uint32_t timestamp_ms = (uint64_t)_timestamp * 1000u / _timescale;
	if (_frameOffset || (frame_bytes > left)) {
		frame_bytes -= _frameOffset;
		ofs += _frameOffset;
		if (frame_bytes > left) {
			frame_bytes = left;
			_frameOffset += left;
		}
		else {
			_frameOffset = 0;
		}
	}
	_mp4Demuxer_c->_fileSeekCb(ofs, _mp4Demuxer_c->_filePriv);
	int32_t rsize = _mp4Demuxer_c->_fileReadCb(data, frame_bytes, _mp4Demuxer_c->_filePriv);
	//int rsize = _io->Read(data, frame_bytes);
	if (!rsize) {
		//LOG_MP4("!rsize");
		return false;
	}
	if (rsize != frame_bytes) {
		//LOG_MP4("rsize != frame_bytes");
		return false;
	}
	size = rsize;
	_position = ofs;
	return true;
}

bool Mp4Demuxer_c::Mp4TrackInfo_c::Read(void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration) {
	if (_readFromPosition) {
		return ReadFragment(data, size, timestamp, duration);
	}
	return ReadNormal(data, size, timestamp, duration);
}

Mp4Demuxer_c::Mp4Demuxer_c() {
}
Mp4Demuxer_c::~Mp4Demuxer_c() {
}
Mp4Ret_t Mp4Demuxer_c::Open() {
	LOG_MP4("v%s", MP4_VERSION);
	if (!_bp.malloc_cb
		|| !_bp.realloc_cb
		|| !_bp.free_cb) {
		return MP4_RET_FAIL;
	}
	if (!_filePriv
		|| !_fileSize
		|| !_fileSeekCb
		|| !_fileReadCb) {
		return MP4_RET_FAIL;
	}

	MP4D_open(&_mp4dDemux, Mp4ReadCallback, this, _fileSize);
	if (!_mp4dDemux.track_count) {
		LOG_MP4("!mp4.track_count");
		return MP4_RET_FAIL;
	}
	FMp4Parser_t& fMp4Parser = _fMp4Parser;
	if (_mp4dDemux.moof_start_pos) {
		fMp4Parser._mp4 = &_mp4dDemux;
		fMp4Parser._moofPosition = _mp4dDemux.moof_start_pos;
		fMp4Parser._tempBuff = 0;
		fMp4Parser._moofIndex = 1;
		fMp4Parser._mp4Demuxer_c = this;
	}
	for (int i = 0; i < _mp4dDemux.track_count; ++i) {
		Mp4TrackInfo_c* trackInfo = &_trackInfo[i];
		char*& codec = trackInfo->codec;
#if 1
		switch (_mp4dDemux.track[i].object_type_indication) {
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
		MP4D_track_t* track = &_mp4dDemux.track[i];
		LOG_MP4("0x%x -> %s, stream_type:0x%x", track->object_type_indication, codec ? codec : " ", track->stream_type);
		switch (track->stream_type) {
		case 0x04: {// VisualStream
			LOG_MP4("wxh=%dx%d", track->SampleDescription.video.width, track->SampleDescription.video.height);
			LOG_MP4("timescale:%d,duration_lo:%d,sample_count:%d", track->timescale, track->duration_lo, track->sample_count);
			if (track->timescale) {
				auto frameRate = track->duration_lo ? (track->sample_count * track->timescale / track->duration_lo) : 20;
				LOG_MP4("frameRate:%d", frameRate);
			}
			break;
		}
		case 0x05: {// AudioStream
			auto rate = track->SampleDescription.audio.samplerate_hz;
			if (!rate) { rate = track->timescale; }
			auto channels = track->SampleDescription.audio.channelcount;
			LOG_MP4("rate:%d,timescale:%d,channel:%d,",
				track->SampleDescription.audio.samplerate_hz,
				track->timescale,
				track->SampleDescription.audio.channelcount);
			break;
		}
		case 0xC0://OtherStream
			LOG_MP4("OtherStream");
			break;
		}

		trackInfo->_trIdx = i;
		trackInfo->_timescale = track->timescale;
		trackInfo->_duration = trackInfo->_timescale ? track->duration_lo * 1000ll / trackInfo->_timescale : 0;
		trackInfo->_extraData = track->dsi;
		trackInfo->_extraSize = track->dsi_bytes;
		trackInfo->_frameCount = track->sample_count;
		trackInfo->_mp4 = &_mp4dDemux;
		trackInfo->_fmp4 = &fMp4Parser;
		trackInfo->_mp4Demuxer_c = this;
		LOG_MP4("timescale:%d,duration:%d,extraData:%p,extraSize:%d,frameCount:%d",
			trackInfo->_timescale,
			trackInfo->_duration,
			trackInfo->_extraData,
			trackInfo->_extraSize,
			trackInfo->_frameCount
		);

		if (_mp4dDemux.moof_start_pos) {
			//trackInfo->_cbRead = &readFragment;
			trackInfo->_readFromPosition = true;
			//_frameCount = 0xffffffff;
		}
		else {
			trackInfo->_readFromPosition = false;
		}
	}
	LOG_MP4("title,%p,%d", _mp4dDemux.tag.title, _mp4dDemux.tag.title_size);
	LOG_MP4("artist,%p,%d", _mp4dDemux.tag.artist, _mp4dDemux.tag.artist_size);
	LOG_MP4("album,%p,%d", _mp4dDemux.tag.album, _mp4dDemux.tag.album_size);

	for (int i = 0; i < _mp4dDemux.track_count; ++i) {
		_frameCntMax = _frameCntMax < _trackInfo[i]._frameCount ? _trackInfo[i]._frameCount : _frameCntMax;
	}

	return MP4_RET_SUCCESS;
}
Mp4Ret_t Mp4Demuxer_c::Set(uint32_t key, void* val) {
	switch (key)
	{
	case Str2Key("file_priv"):
		_filePriv = val;
		return MP4_RET_SUCCESS;
	case Str2Key("file_size"):
		_fileSize = (uint32_t)val;
		return MP4_RET_SUCCESS;
	case Str2Key("file_seek_cb"):
		_fileSeekCb = (int32_t(*)(uint32_t offset, void* priv))val;
		return MP4_RET_SUCCESS;
	case Str2Key("file_read_cb"):
		_fileReadCb = (int32_t(*)(void* buffer, uint32_t size, void* priv))val;
		return MP4_RET_SUCCESS;
	default:break;
	}
	if (Mp4Base_c::Set(key, val) == GASF_RET_SUCCESS) return MP4_RET_SUCCESS;
	return MP4_RET_FAIL;
}
Mp4Ret_t Mp4Demuxer_c::Get(uint32_t key, void* val) {
#if 0
	switch (key)
	{
	case Str2Key("rate"):
		if (_stage < Stage_e::STAGE_USER_COMMENT_HEAD)return MP4_RET_FAIL;
		if (val) *(uint32_t*)val = _rate;
		return MP4_RET_SUCCESS;
	default:break;
	}
#endif
	if (Mp4Base_c::Get(key, val) == GASF_RET_SUCCESS) return MP4_RET_SUCCESS;
	return MP4_RET_FAIL;
}
Mp4Ret_t Mp4Demuxer_c::Receive(Mp4Data_c& iData) {
	return MP4_RET_SUCCESS;
}
Mp4Ret_t Mp4Demuxer_c::Generate(Mp4Data_c& oData) {
	if(_frameCnt> _frameCntMax) 
		return MP4_RET_FAIL;
	for (int i = 0; i < _mp4dDemux.track_count; ++i) {
		if (_frameCnt < _trackInfo[i]._frameCount) {
			Mp4TrackInfo_c* trackInfo = &_trackInfo[i];
			unsigned timestamp = 0;
			unsigned duration = 0;
			unsigned frame_bytes = 0;
			const uint32_t readBufByte = 100 * 1024;
			static uint8_t readBuf[readBufByte];
			uint32_t readBufByteActual = readBufByte;
			trackInfo->Read(readBuf, readBufByteActual, timestamp, duration);
			LOG_MP4("[%d,%s][%d]%p,%d", i, trackInfo->codec, _frameCnt, readBuf, readBufByteActual);

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
	++_frameCnt;
	return MP4_RET_SUCCESS;
}
Mp4Ret_t Mp4Demuxer_c::Close() {
	return MP4_RET_SUCCESS;
}





