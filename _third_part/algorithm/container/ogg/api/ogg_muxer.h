#pragma once
#include "ogg.h"
#include "ogg_api_private.h"
#include "gaapi_gaf.h"
using namespace ogg_gaapi_ns;

namespace ogg_api_ns {
	class OggMuxer_c:public GaapiGaf_c
	{
		using Base_c = GaapiGaf_c;
		enum class Stage_e {
			STAGE_NONE = 0,
			STAGE_ID_HEAD,
			STAGE_USER_COMMENT_HEAD,
			STAGE_DATA_HEAD,
		};
	public:
		OggMuxer_c() {}
		virtual ~OggMuxer_c() {}
	public:
		virtual OggRet_t Open()override;
		virtual OggRet_t Set(uint32_t key, void* val)override;
		virtual OggRet_t Get(uint32_t key, void* val)override;
		//virtual OggRet_t Run(GaapiData_c& iData, GaapiData_c& oData)override;
		virtual OggRet_t Receive(GaapiData_c& iData)override;
		virtual OggRet_t Generate(GaapiData_c& oData)override;
		virtual OggRet_t Close()override;

	public:
		ogg_sync_state   _oggSyncS;
		ogg_page         _oggPage;
		ogg_stream_state _oggStreamS;
		ogg_packet       _oggPacket;
		uint8_t _idPageCache[512];
		OggPage_t _idPage;
		uint8_t _userCommentPageCache[512];
		OggPage_t _userCommentPage;
		Stage_e _stage = Stage_e::STAGE_NONE;
		int32_t _page_out_fill_byte = 4096;
		int32_t _byte_per_sample = 0;
		int32_t _acc_byte = 0;
		int32_t _granulepos = 0;
		int32_t _packetno = 0;
		OggMuxerApiParam_t* _param;

		bool _isBos = true;
		bool _isEos = false;
		bool _isReceiveEnd = false;
		bool _isGenrateEnd = false;
	};
};

