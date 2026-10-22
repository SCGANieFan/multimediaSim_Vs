#include "MediaH264.h"
#include "SmfFCC.h"
#include "SmfHash.h"
#include "BitStream.h"
#include "smf_debug.h"
using namespace smf;

static bool check_start_code(const uint8_t* ptr, int& len) {
	len = 0;
	if(!ptr[0] && !ptr[1] && ((!ptr[2] && ptr[3] == 0x1)||(ptr[2] == 0x1))) {
		len = 4 - ptr[2];
		return true;
	}
	return false;
}

MediaH264::MediaH264() { 
	_codec = fcc32("h264");
	_size = sizeof(MediaH264);
	_vbr = 0;
	_profile = 0;
	_gop = 0;
	_streamType = 0;
	_sps = nullptr;
	_pps = nullptr;
}
bool MediaH264::setX(uint32_t key, void* val) {
	switch (key) {
	case Hash("vbr"): _vbr = (uint32_t)val;	return true;
	case Hash("profile"): _profile = (uint32_t)val;	return true;
	case Hash("format"): _format = (uint32_t)val;	return true;
	case Hash("gop"): _gop = (uint32_t)val;	return true;
	case Hash("streamType"): _streamType = (uint8_t)(uint32_t)val; return true;
	}
	return MediaVideo::setX(key, val);
}
bool MediaH264::getX(uint32_t key, void* val) const {
	switch (key) {
	case Hash("vbr"): *(uint32_t*)val = _vbr;	return true;
	case Hash("profile"): *(uint32_t*)val = _profile;	return true;
	case Hash("format"): *(uint32_t*)val = _format;	return true;
	case Hash("gop"): *(uint32_t*)val = _gop;	return true;
	}
	return MediaVideo::getX(key, val);
}
bool MediaH264::SetExtraData(void* data, uint32_t size) {
	return IMedia::SetExtraData(data, size) && ParseExtraData();
}
bool MediaH264::GetExtraData(void*& data, uint32_t& size) {
	if (IMedia::GetExtraData(data, size))return true;
	return false;
}
#if 0
static inline uint32_t Read(BsReader& bs) {//entropyDec
	int c = 0;
	while (!bs.Read(1)) {
		c++;
	}
	uint32_t v = bs.Read(c);
	return (v | (1 << c)) -1;
}
static inline int32_t Reads(BsReader& bs) {//entropyDec
	int c = 0;
	while (!bs.Read(1)) {
		c++;
	}
	int v = bs.Read(c-1);
	v |= (1 << c);
	return bs.Read(1) ? -v : v;
}
static inline uint32_t Read(BsReader& bs, int bits) {
	return bs.Read(bits);
}
#endif

bool MediaH264::ParseExtraData() {
	returnIfErrC(false, !_extraData);
	returnIfErrC(false, !_extraSize);
	auto ptr = _extraData;
	auto end = ptr + _extraSize;
	int num = 0;
	if (check_start_code(ptr, num)) {
		auto nal = 0;
		num = 0;
		while (ptr + 5 < end) {
			if (check_start_code(ptr, num)) {
				if (nal == 7) {
					_sps_len = ptr - _sps;
					nal = 0;
				}
				else if (nal == 8) {
					_pps_len = ptr - _pps;
					nal = 0;
				}
				switch (ptr[num] & 0x1f) {
				case 7://sps
					_sps = ptr + num;
					nal = 7;
					break;
				case 8://pps
					_pps = ptr + num;
					nal = 8;
					break;
				}
				ptr += num;
			}
			ptr++;
		}
		if (nal == 7) {
			_sps_len = ptr - _sps;
			nal = 0;
		}
		else if (nal == 8) {
			_pps_len = ptr - _pps;
			nal = 0;
		}
		_streamType = h264_stream_e::H264StreamAnnexb;
	}
	else {
		auto len0 = Read32BE(ptr);
		auto nal0 = ptr[4] & 31;
		auto len1 = Read32BE(ptr + 4 + len0);
		auto nal1 = ptr[len0 + 4 + 4] & 31;
		returnIfErrC(false, !((nal0 == 7 && nal1 == 8) || (nal0 == 8 && nal1 == 7)));
		if (nal0 == 7) {
			_sps = _extraData + 4;
			_sps_len = len0;
			_pps = _extraData + 4 + 4 + len0;
			_pps_len = len1;
		}
		else {
			_pps = _extraData + 4;
			_pps_len = len0;
			_sps = _extraData + 4 + 4 + len0;
			_sps_len = len1;
		}
		_streamType = h264_stream_e::H264StreamAvcc;
	}
	_extraSize = _sps_len + _pps_len + 4 + 4;
	dbgTestPXL("sps:%p,%u,pps:%p,%u", _sps, _sps_len, _pps, _pps_len);
	return ParseSPS() && ParsePPS();
}
#if 0
bool MediaH264::ParseSPS() {
	returnIfErrC(false, !_sps);
	returnIfErrC(false, !_sps_len);
	BsReader bs(_sps, _sps_len);

	_profile = Read(bs, 8);//profile_idc
	Read(bs, 5);//flags
	Read(bs, 3);//flags_zero
	_level = Read(bs, 8);//constraint_setX_flag
	Read(bs);//seq_parameter_set_id;
	switch (_profile) {
	case 100:
	case 110:
	case 122:
	case 244:
	case 44:
	case 83:
	case 86:
	case 118:
	case 128: {
		auto chroma_format_idx = Read(bs);//chroma_format_idx
		if (chroma_format_idx == 3) {
			Read(bs, 1);//separate_colour_plane_flag;
		}
		Read(bs);//bit_depth_luma_minus8
		Read(bs);//bit_depth_chroma_minus8
		Read(bs, 1);//qpprime_y_zero_transform_bypass_flag
		auto seq_scaling_matrix_present_flag = Read(bs, 1);//seq_scaling_matrix_present_flag
		if (seq_scaling_matrix_present_flag) {
			for (int i = 0; i < ((chroma_format_idx != 3) ? 8 : 12); i++) {
				auto seq_scaling_list_present_flag = Read(bs, 1);//seq_scaling_list_present_flag[i];
				if (seq_scaling_list_present_flag) {
					if (i < 6);
					else;
				}
			}
		}
		break;
	}
	default:
		break;
	}
	Read(bs);//log2_max_frame_num_minus4
	auto pic_order_cnt_type = Read(bs);//pic_order_cnt_type
	switch (pic_order_cnt_type) {
	case 0:
		Read(bs);//log2_max_pic_order_cnt_1sb_minus4
		break;
	case 1: {
		Read(bs, 1);//delta_pic_order_always_zero_flag
		Reads(bs);//offset_for_non_ref_pic
		Reads(bs);//offset_for_top_to_bottom_field
		auto num_ref_frames_in_pic_order_cnt_cycle = Read(bs);//num_ref_frames_in_pic_order_cnt_cycle
		for (int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++) {
			Reads(bs);//offset_for_ref_frame[i]
		}
		break;
	}
	default:
		break;
	}
	Read(bs);//max_num_ref_frames
	Read(bs, 1);//gaps_in_frame_num_value_allowed_flag
	auto pic_width_in_mbs_minus1 = Read(bs);//pic_width_in_mbs_minus1
	auto pic_height_in_map_units_minus1 = Read(bs);//pic_height_in_map_units_minus1
	auto frame_mbs_only_flag = Read(bs, 1);//frame_mbs_only_flag
	if (!frame_mbs_only_flag)
		Read(bs, 1);//mb_adaptive_frame_field_flag
	Read(bs, 1);//direct_8x8_inference_flag
	auto frame_cropping_flag = Read(bs, 1);//frame_cropping_flag
	if (frame_cropping_flag) {
		Read(bs);//frame_crop_left_offset;
		Read(bs);//frame_crop_right_offset;
		Read(bs);//frame_crop_top_offset;
		Read(bs);//frame_crop_bottom_offset;
	}
	auto vui_parameters_present_flg = Read(bs, 1);//vui_parameters_present_flg
	if (vui_parameters_present_flg) {

	}
	//
	//MaxFrameNum = 2^(log2_max_frame_num_minus4 + 4);
	_width = (pic_width_in_mbs_minus1 + 1) * 16;
	_height = (2- frame_mbs_only_flag)*(pic_height_in_map_units_minus1 + 1) * 16;
	dbgTestPXL("%u,%u", _width, _height);
	return true;
}
#endif

bool MediaH264::ParsePPS() {
	returnIfErrC(false, !_pps);
	returnIfErrC(false, !_pps_len);
#if 0
	BsReader bs(_pps, _pps_len);
	auto pic_parameter_set_id = Read(bs);
	auto seq_parameter_set_id = Read(bs);
	auto entropy_coding_mode_flag = Read(bs,1);
	auto bottom_field_pic_order_in_frame_present_flag = Read(bs,1);
	auto num_slice_groups_minus1 = Read(bs);
	if (num_slice_groups_minus1 > 0) {
		auto slice_group_map_type = Read(bs);
		switch (slice_group_map_type){
		case 0:
			for (int iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++) {
				auto run_length_minus1 = Read(bs);
			}
			break;
		case 2:
			for (int iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++) {
				auto top_left = Read(bs);
				auto bottom_right = Read(bs);
			}
			break;
		case 3:
		case 4:
		case 5: {
			auto slice_group_change_direction_flag = Read(bs,1);
			auto slice_group_change_rate_minus1 = Read(bs);
			break;
		}
		case 6:
			auto pic_size_in_map_units_minus1 = Read(bs);
			for (int iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++) {
				auto slice_group_id = Read(bs);
			}
			break;
		}
	}
	auto num_ref_idx_10_default_avtive_minus1 = Read(bs);
	auto num_ref_idx_11_default_avtive_minus1 = Read(bs);
	auto weighted_pred_flag = Read(bs,1);
	auto weighted_bipred_idc = Read(bs,2);
	auto pic_init_qp_minus26 = Reads(bs);
	auto pic_init_qs_minus26 = Reads(bs);
	auto chroma_qp_index_offset = Reads(bs);
	auto deblocking_filter_control_present_flag = Read(bs,1);
	auto constrained_intra_pred_flag = Read(bs,1);
	auto redundant_pic_cnt_present_flag = Read(bs,1);
	if (more_rbsp_data()) {
		auto transform_8x8_mode_flag = Read(bs, 1);
		auto pic_scaling_matrix_present_flag = Read(bs, 1);
		if (pic_scaling_matrix_present_flag) {
			for (int i = 0; i < 6 + ((chroma_format_idx != 3) ? 2 : 6) * transform_8x8_mode_flag; i++) {
				auto pic_scaling_list_present_flag_i = Read(bs, 1);
				if (pic_scaling_list_present_flag_i) {
					if (i < 6) {
						//scaling_list(ScalingList8x8[i],64,UseDefaultScalingMatrix8x8Flag_i
					}
					else {
						//scaling_list(ScalingList8x8[i-6],64,UseDefaultScalingMatrix8x8Flag_i_6
					}
				}
			}
		}
		auto second_chroma_qp_index_offset = Reads(bs);
	}
	//rbsp_trailing_bits()
#endif
	return true;
}
bool MediaH264::JumpToDataAVC(uint8_t*& data, int& size) {
	returnIfErrC(0, !data);
	returnIfErrC(0, size < 5);
	while (size > 5) {
		switch (data[4] & 0x1f) {
		case H264_IDR:
		case H264_PDR:
			data += 4;
			size -= 4;
			return true;
		default:
		{
			uint32_t len = Read32BE(data) + 4;
			data += len;
			size -= len;
			break;
		}
		}
	}
	return false;
}
bool MediaH264::JumpToDataBitStream(uint8_t*& data, int& size) {
	returnIfErrC(false, !data);
	returnIfErrC(false, size < 5);
	int num = 0;
	while (size > 5) {
		if (check_start_code(data, num)) {
			data += num;
			size -= num;
			switch (data[0] & 0x1f) {
			case H264_IDR:
			case H264_PDR:
				return true;
			default:
				break;
			}
		}
		size--;
		data++;
	}
	return false;
}
bool MediaH264::JumpToData(uint8_t*& data, int& size, const char* pack) {
	returnIfErrC(0, !data);
	returnIfErrC(0, size < 5);
	if (!pack)pack = "bitstream";
	switch (Hash(pack)) {
	case Hash("avc"):
		return JumpToDataAVC(data, size);
	case Hash("bitstream"):
		return JumpToDataBitStream(data, size);
	default:
		break;
	}
	return false;
}
bool MediaH264::JumpToData(uint8_t*& data, int& size, uint8_t pack) {
	returnIfErrC(0, !data);
	if (!pack)pack = H264StreamAnnexb;
	switch (pack) {
	case H264StreamAvcc:
		return JumpToDataAVC(data, size);
	case H264StreamAnnexb:
		return JumpToDataBitStream(data, size);
	case H264StreamRaw:
		return true;
	default:
		break;
	}
	return false;
}
bool MediaH264::JumpToData(uint8_t*& data, int& size) {
	return JumpToData(data, size, _streamType);
}