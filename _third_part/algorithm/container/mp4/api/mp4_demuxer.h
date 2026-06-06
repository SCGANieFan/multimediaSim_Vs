#pragma once
#include "gasf.h"
#include "mp4_common.h"
#include "minimp4.h"


namespace mp4_ns {
	class Mp4Demuxer_c :public Mp4Base_c
	{
	public:

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

		struct FMp4Parser_t {
			MP4D_demux_t* _mp4 = 0;
			int64_t _moofPosition = 0;
			uint32_t _moofIndex = 0;
			void* _tempBuff = 0;
			uint32_t ret_continue;
			Mp4Demuxer_c* _mp4Demuxer_c = 0;
		};

		class Mp4TrackInfo_c {
		public:
			Mp4TrackInfo_c() {}
			~Mp4TrackInfo_c() {}
		public:
			bool Read(void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration);
			bool ReadFragment(void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration);
			bool ReadNormal(void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration);

		public:
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
			Mp4Demuxer_c* _mp4Demuxer_c = 0;
			//bool (*_cbRead)(Mp4TrackInfo* info, void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration) = 0;
			bool (*_cbRead)(Mp4TrackInfo_c* info, void* data, uint32_t& size, uint32_t& timestamp, uint32_t& duration) = 0;
			bool _readFromPosition = false;
		};

	public:
		Mp4Demuxer_c();
		virtual ~Mp4Demuxer_c();
	public:
		virtual Mp4Ret_t Open()override;
		virtual Mp4Ret_t Set(uint32_t key, void* val)override;
		virtual Mp4Ret_t Get(uint32_t key, void* val)override;
		//virtual Mp4Ret_t Run(Mp4Data_c& iData, Mp4Data_c& oData)override;
		virtual Mp4Ret_t Receive(Mp4Data_c& iData)override;
		virtual Mp4Ret_t Generate(Mp4Data_c& oData)override;
		virtual Mp4Ret_t Close()override;
	public:
		void* _filePriv = 0;
		uint32_t _fileSize = 0;
		int32_t(*_fileSeekCb)(uint32_t offset, void* priv) = 0;
		int32_t(*_fileReadCb)(void* buffer, uint32_t size, void* priv) = 0;
		MP4D_demux_t _mp4dDemux;
		uint32_t _frameCntMax = 0;
		uint32_t _frameCnt = 0;
		Mp4TrackInfo_c _trackInfo[2];
		FMp4Parser_t _fMp4Parser;
	};
}
