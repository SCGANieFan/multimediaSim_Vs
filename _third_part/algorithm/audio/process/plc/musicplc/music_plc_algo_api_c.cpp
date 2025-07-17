#include "music_plc_algo_api_c.h"
#include "plc_register_c.h"

EXTERNC{

void plc_register_music_plc_int16() {
	plc_register_c::Register<music_plc_algo_api_c<int16_t>>("mplc_i16");
}
void plc_register_music_plc_int32() {
	plc_register_c::Register<music_plc_algo_api_c<int16_t>>("mplc_i32");
}
void plc_register_music_plc_intf32() {
	plc_register_c::Register<music_plc_algo_api_c<int16_t>>("mplc_if32");
}

}