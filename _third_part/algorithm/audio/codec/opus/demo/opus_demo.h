#include <stdint.h>

void* opus_demo_encoder_init(uint32_t rate, uint16_t channels, uint8_t width, uint32_t bitrate, uint32_t frameDMs, bool haveHead);
bool opus_demo_encoder_run(void* enc, uint8_t* pcm, int32_t *pcmByte, uint8_t* encodedFrame, int32_t* encodedFrameByte);
bool opus_demo_encoder_deinit(void* enc);

void* opus_demo_decoder_init(uint32_t rate, uint16_t channels, uint8_t width);
bool opus_demo_decoder_run(void* dec, uint8_t* encodedOneFrame, uint32_t encodedOneFrameByte, uint8_t* decodecPcm, int32_t* decodecPcmByte, bool isDoPlc);
bool opus_demo_decoder_deinit(void* dec);

