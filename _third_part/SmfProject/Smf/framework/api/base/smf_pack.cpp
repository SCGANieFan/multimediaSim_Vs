#include "smf_common.h"

EXTERNC unsigned smf_pack_crc(void* pack0) {
	uint8_t* ptr = (uint8_t *) pack0;
	unsigned v0 = ptr[1] ^ ptr[2] ^ ptr[3];
	unsigned v1 = (ptr[0] & 0xf) ^ (v0 & 0xf) ^ ((v0 >> 4) & 0xf);
	return v1;
}

EXTERNC bool smf_pack_crc_check(void* pack0) {
	uint8_t* ptr = (uint8_t*)pack0;
	unsigned v0 = smf_pack_crc(pack0);
	unsigned vv = (ptr[0] >> 4) & 0xf;
	return vv == v0;
}

EXTERNC void smf_pack_crc_fill(void* pack0) {
	uint8_t* ptr = (uint8_t*)pack0;
	unsigned v0 = smf_pack_crc(pack0);
	ptr[0] = (ptr[0] & 0xf) | ((v0 & 0xf) << 4);
}
