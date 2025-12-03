#include <stdint.h>

void* ogg_demo_muxer_init(uint32_t rate, uint16_t channels, uint8_t width, uint32_t frameDMs);
bool ogg_demo_muxer_run(void* ogg, uint8_t* data, int32_t* dataByte, uint8_t* oggMuxedData, int32_t* oggMuxedDataByte, bool isEos);
bool ogg_demo_muxer_deinit(void* ogg);

