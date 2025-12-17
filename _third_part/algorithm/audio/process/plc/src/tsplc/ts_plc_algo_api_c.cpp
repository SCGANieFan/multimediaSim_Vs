#include "ts_plc_algo_api_c.h"
#include "plc_register_c.h"
#include "signal_processing_library.h"

plc_api_ret_t ts_plc_algo_api_c::create(plc_api_param_t* plc_api_param, plc_base_port_c* plc_base_port) {

	LOG(plc_base_port->print_cb, "plc api musicplc, (%p,%d,%d,%d),(%d{%d,%d,%d,%d,%d})",
		plc_base_port,
		plc_api_param->fs_hz,
		plc_api_param->channels,
		plc_api_param->frame_samples,
		plc_api_param->param_set,
		plc_api_param->ts_plc.recv);

	if (!plc_base_port
		|| plc_api_param->channels < 1)
		return TS_PLC_API_RET_FAIL;
	if (plc_api_param->frame_samples < 1
		|| plc_api_param->sbc_plc.codec_type > SBC_PLC_CODEC_TYPE_MAX
		|| plc_api_param->sbc_plc.search_size < 0
		|| plc_api_param->sbc_plc.match_size < 0
		|| plc_api_param->sbc_plc.olal < 0
		|| plc_api_param->sbc_plc.recover < 0)
		return TS_PLC_API_RET_FAIL;
	_base_porting = plc_base_port;
	i32 ret = init(plc_api_param);
	if(ret != TS_PLC_API_RET_SUCCESS){
		LOG(plc_base_port->print_cb, "plc api create fail, %d", ret);
		destory();
	}
	return ret;
}

plc_api_ret_t ts_plc_algo_api_c::set(plc_api_set_e choose, void* val) {
	return TS_PLC_API_RET_SUCCESS;
}

plc_api_ret_t ts_plc_algo_api_c::get(plc_api_get_e choose, void* val) {
	return TS_PLC_API_RET_SUCCESS;
}

plc_api_ret_t ts_plc_algo_api_c::run(uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, uint16_t is_lost)
{
#if 0
	if (is_lost == false) {
		if (!in
			|| inLen < (frame_samples * _info._bytes_per_sample))
			return TS_PLC_API_RET_FAIL;
	}
	if (*outLen < frame_samples * _info._bytes_per_sample)
		return TS_PLC_API_RET_FAIL;
#endif
	run(in, out, outLen, is_lost ? true : false);
	if (inUsed) *inUsed = inLen;
	return TS_PLC_API_RET_SUCCESS;
}


plc_api_ret_t ts_plc_algo_api_c::destory() {
	deinit();
	_mm.FreeAll();
	return TS_PLC_API_RET_SUCCESS;
}


i32 ts_plc_algo_api_c::init(plc_api_param_t* plc_api_param) {
	_width = plc_api_param->data_type == plc_api_data_type_e::PLC_API_DATA_TYPE_SHORT_16 ? 2 : 4;
	_mm.Init(_base_porting);
	_channels = plc_api_param->channels;
	_byte_per_frame = plc_api_param->frame_samples * _width * _channels;
	_rate = plc_api_param->fs_hz;
	_frame_sample = plc_api_param->frame_samples;
	_hist_byte = 20 * (_rate / 1000) * _width * _channels;
	WebRtcSpl_Init();
	_pe = (PreemptiveExpand*)_mm.malloc(sizeof(PreemptiveExpand));
	if (!_pe) { return TS_PLC_API_RET_FAIL; }
	new(_pe) PreemptiveExpand(_rate, _channels, 0);

	_algo_buf = (AudioMultiVector*)_mm.malloc(sizeof(AudioMultiVector));
	if(!_algo_buf) { return TS_PLC_API_RET_FAIL; }
	new(_algo_buf) AudioMultiVector(_channels);

	i32 buf_byte = _hist_byte + 4 * _byte_per_frame + 40 * (_rate / 1000) * _width * _channels;
	u8* buf = (u8*)_mm.malloc(buf_byte);
	PLC_MEM_SET(buf, 0, buf_byte);
	if (!buf) { return TS_PLC_API_RET_FAIL; }
	_tmp_buff.init(buf, buf_byte);
	//_tmp_buff.append(_hist_byte);
	return TS_PLC_API_RET_SUCCESS;
}


i32 ts_plc_algo_api_c::deinit() {
	if (_pe) {
		_pe->~PreemptiveExpand();
		_mm.free(_pe);
		_pe = 0;
	}
	if (_algo_buf) {
		_algo_buf->~AudioMultiVector();
		_mm.free(_algo_buf);
		_algo_buf = 0;
	}
	if (_tmp_buff.get_buf()) {
		_mm.free(_tmp_buff.get_buf());
		_tmp_buff.deinit();
	}
	
	return TS_PLC_API_RET_SUCCESS;
}

ts_plc_algo_api_c* ts_plc_algo_api_i16_c::create_hd(plc_base_port_c* plc_base_port) {
	int size = sizeof(ts_plc_algo_api_i16_c);
	ts_plc_algo_api_i16_c* plc_api = (ts_plc_algo_api_i16_c*)plc_base_port->malloc(size);
	if (!plc_api) {
		return 0;
	}
	new(plc_api) ts_plc_algo_api_i16_c();
	return plc_api;
}

i32 ts_plc_algo_api_i16_c::run(uint8_t* in, uint8_t* out,i32 *out_len, b1 is_lost) {
	if (is_lost){
		_lost_cnt++;
		if (_tmp_buff.get_size() < _byte_per_frame) {
			assert(0);
			i32 append_byte = _byte_per_frame - _tmp_buff.get_size();
			PLC_MEM_SET(_tmp_buff.get_left_data(), 0, append_byte);
			_tmp_buff.append(append_byte);
		}
	}
	else {
		if (_lost_cnt) {
			_lost_cnt = 0;
#if 1
			if (_tmp_buff.get_size() < _byte_per_frame) {
				assert(0);
				i32 append_byte = _byte_per_frame - _tmp_buff.get_size();
				PLC_MEM_SET(_tmp_buff.get_left_data(), 0, append_byte);
				_tmp_buff.append(append_byte);
			}
			i16* p_down = (i16*)(_tmp_buff.get_buf() + _tmp_buff.get_size() - _byte_per_frame);
			i16* p_up = (i16*)in;
			i16* p_dst = p_down;
			i32 one_q12 = ((i32)1 << 12);
			i32 fac_up_step_q12 = one_q12 / _frame_sample;
			i32 fac_up_q12 = 0;
#if 1
			if (_channels == 1) {
				for (u16 n = 0; n < _frame_sample; n++) {
					p_dst[n] = (p_down[n] * (one_q12 - fac_up_q12) + p_up[n] * fac_up_q12) >> 12;
					fac_up_q12 += fac_up_step_q12;
				}
			}
			else if (_channels == 2) {
				for (u16 n = 0; n < _frame_sample * 2; n += 2) {
					p_dst[n + 0] = (p_down[n + 0] * (one_q12 - fac_up_q12) + p_up[n + 0] * fac_up_q12) >> 12;
					p_dst[n + 1] = (p_down[n + 1] * (one_q12 - fac_up_q12) + p_up[n + 1] * fac_up_q12) >> 12;
					fac_up_q12 += fac_up_step_q12;
				}
			}
#else
			for (u16 n = 0; n < _frame_sample; n++) {
				p_dst[n] = 0;
			}
#endif
#else
			_tmp_buff.append(in, _byte_per_frame);
#endif
		}
		else {
			if (_tmp_buff.get_size() < 2 * _byte_per_frame) {
			//if (1) {
				PLC_MEM_CPY(_tmp_buff.get_left_data(), in, _byte_per_frame);
				i32 appen_sample = _frame_sample;
				for (u16 n = 0; n < 1; n++) {
					i32 ret = 0;
					size_t change_samples = 0;
					size_t len = 0;
					ret = _pe->Process((const int16_t*)_tmp_buff.get_left_data(), appen_sample, 0, _algo_buf, &change_samples);
					len = _algo_buf->ReadInterleaved(_tmp_buff.get_left_size(), (int16_t*)_tmp_buff.get_left_data());
					appen_sample = len / _channels;
					_algo_buf->PopFront(appen_sample / _channels);
					if (_tmp_buff.get_size() + appen_sample*_width >= 3 * _byte_per_frame) { break; }
				}
				_tmp_buff.append(appen_sample * _width);
			}
			else {
				_tmp_buff.append(in, _byte_per_frame);
			}
		}
	}
	PLC_MEM_CPY(out, _tmp_buff.get_data(), _byte_per_frame);
	_tmp_buff.used(_byte_per_frame);
	_tmp_buff.clear(_byte_per_frame);
	if (out_len) *out_len = _byte_per_frame;
	return TS_PLC_API_RET_SUCCESS;
}


EXTERNC{

void plc_register_ts_plc_int16() {
	plc_register_c::Register<ts_plc_algo_api_i16_c>("tsplc_i16");
}
}