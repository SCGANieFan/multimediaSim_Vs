#pragma once
#include <stdint.h>
typedef struct {
	uint32_t size;
	uint32_t alac;
	uint32_t _nused0;
	uint32_t max_sample_per_frame;

	uint8_t _unused1;
	uint8_t sample_size;
	uint8_t rice_history_mult;
	uint8_t rice_initial_history;

	uint8_t rice_kmodifier;
	uint8_t channels;
	uint16_t _unused2;

	uint32_t max_coded_frame_size;
	uint32_t bitrate;
	uint32_t samplerate;
}alac_extradata_t;