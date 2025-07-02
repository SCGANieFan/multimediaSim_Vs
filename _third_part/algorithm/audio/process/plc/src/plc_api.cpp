#include "plc_api_c.h"

EXTERNC{
plc_api_ret_t plc_api_create(void** p_hd, plc_api_param_t* param){
	return plc_api_c::create(p_hd,param);
}

plc_api_ret_t plc_api_run(void* hd, uint8_t* in, int32_t in_len, int32_t* in_used, uint8_t* out, int32_t* p_out_len, uint16_t is_lost){
	return plc_api_c::run(hd,in,in_len,in_used,out,p_out_len,is_lost);
}

plc_api_ret_t plc_api_set(void* hd, plc_api_set_e choose, void* val) {
	return plc_api_c::set(hd,choose,val);
}

plc_api_ret_t plc_api_get(void* hd, plc_api_get_e choose, void* val) {
	return plc_api_c::get(hd,choose,val);
}

plc_api_ret_t plc_api_destory(void* hd){
	return plc_api_c::destory(hd);
}

void plc_api_register_music_plc_int16() {
	extern void plc_register_music_plc_int16();
	plc_register_music_plc_int16();
}

void plc_api_register_music_plc_int32() {
	extern void plc_register_music_plc_int32();
	plc_register_music_plc_int32();
}

void plc_api_register_music_plc_f32() {
	extern void plc_register_music_plc_intf32();
	plc_register_music_plc_intf32();
}

void plc_api_register_sbc_plc_i16() {
	extern void plc_register_sbc_plc_int16();
	plc_register_sbc_plc_int16();
}

void plc_api_register_ts_plc_i16() {
	extern void plc_register_ts_plc_int16();
	plc_register_ts_plc_int16();
}


}
