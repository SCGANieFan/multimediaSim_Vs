#include "VideoFormatList.h"
#include <xtensa/tie/xt_hifi2.h>
using namespace smf;
using namespace smf::video;

static inline ae_int16x4 Packet(ae_int32x2 a, ae_int32x2 b, ae_int32x2 c, ae_int32x2 d) {
	ae_int16x4 m2 = -32768;
	ae_int16x4 n1 = 0xff00;
	ae_int16x4 n2 = 0x00ff;
	a = AE_SLAA32S(a, 16);
	b = AE_SLAA32S(b, 16);
	c = AE_SLAA32S(c, 16);
	d = AE_SLAA32S(d, 16);
	ae_int16x4 t1 = AE_TRUNC16X4F32(a, b);
	ae_int16x4 t2 = AE_TRUNC16X4F32(c, d);
	t1 = t1 + m2;
	t1 = t1 >> 8;
	t1 = t1 & n2;
	t2 = t2 + m2;
	t2 = t2 & n1;
	t2 = t1 | t2;
	return t2;
}

static bool convert(VideoFormat* vf, void* idata, void*& odata) {
	// unsigned ticks = get_ticks();
	unsigned height = vf->_ih;
	unsigned height_2 = height >> 1;
	unsigned width = vf->_iw;
	unsigned width_2 = width >> 1;
	unsigned width_4 = width >> 2;
	unsigned width_8 = width >> 3;
	unsigned widthN4 = width << 2;
	unsigned total = height * width;
	ae_int16x4* inY1 = (ae_int16x4*)idata;
	ae_int16x4* inY2 = inY1 + width_4;
	ae_int16x4* inU = inY1 + (total >> 2);
	ae_int16x4* inV = inU + (total >> 4);
	uint8_t* outZA = (uint8_t*)odata;
	uint8_t* outZB = (uint8_t*)odata + widthN4;
	ae_int16x4* outA = (ae_int16x4*)outZA;
	ae_int16x4* outB = (ae_int16x4*)outZB;
	ae_int16x4 vecInYA, vecInYB, vecInU, vecInV;
	// ae_int16x4 vecTmpC1 = 128;
	ae_int16x4 vecTmpC2 = 16;
	ae_int16x4 vecCoeY = 298;
	ae_int16x4 vecCoeU_R = 409;
	ae_int16x4 vecCoeU_G = -208;
	ae_int16x4 vecCoeV_G = -100;
	ae_int16x4 vecCoeV_B = 516;
	for (int i = 0; i < height_2; i++) {
		for (int j = 0; j < width_8; j++) {
			AE_L16X4_IP(vecInU, inU, 8);
			AE_L16X4_IP(vecInV, inV, 8);
			ae_int32x2 vecTmpU01, vecTmpU23;
			AE_MUL16X4(vecTmpU01, vecTmpU23, vecInU, vecCoeU_R);
			ae_int32x2 vecTmpUV01, vecTmpUV23;
			AE_MUL16X4(vecTmpUV01, vecTmpUV23, vecInU, vecCoeU_G);
			AE_MULA16X4(vecTmpUV01, vecTmpUV23, vecInV, vecCoeV_G);
			ae_int32x2 vecTmpV01, vecTmpV23;
			AE_MUL16X4(vecTmpV01, vecTmpV23, vecInV, vecCoeV_B);

			AE_L16X4_IP(vecInYA, inY1, 8);
			AE_L16X4_IP(vecInYB, inY1, 8);
			vecInYA = vecInYA - vecTmpC2;
			vecInYB = vecInYB - vecTmpC2;

			ae_int32x2 vecTmpY01, vecTmpY23;
			ae_int32x2 vecTmpY45, vecTmpY67;
			AE_MUL16X4(vecTmpY01, vecTmpY23, vecInYA, vecCoeY);
			AE_MUL16X4(vecTmpY45, vecTmpY67, vecInYB, vecCoeY);

			ae_int32x2 vecTmpY02 = AE_SEL32_HH(vecTmpY01, vecTmpY23);
			ae_int32x2 vecTmpY13 = AE_SEL32_LL(vecTmpY01, vecTmpY23);
			ae_int32x2 vecTmpY46 = AE_SEL32_HH(vecTmpY45, vecTmpY67);
			ae_int32x2 vecTmpY57 = AE_SEL32_LL(vecTmpY45, vecTmpY67);

			ae_int32x2 vecOutA08 = vecTmpY02 + vecTmpV01;
			ae_int32x2 vecOutA19 = vecTmpY02 + vecTmpUV01;
			ae_int32x2 vecOutA2a = vecTmpY02 + vecTmpU01;
			ae_int32x2 vecOutA3b = 0xff00;
			ae_int32x2 vecOutA4c = vecTmpY13 + vecTmpV01;
			ae_int32x2 vecOutA5d = vecTmpY13 + vecTmpUV01;
			ae_int32x2 vecOutA6e = vecTmpY13 + vecTmpU01;
			ae_int32x2 vecOutA7f = 0xff00;

			ae_int32x2 vecOutA02 = AE_SEL32_HH(vecOutA08, vecOutA2a);
			ae_int32x2 vecOutA46 = AE_SEL32_HH(vecOutA4c, vecOutA6e);
			ae_int32x2 vecOutA13 = AE_SEL32_HH(vecOutA19, vecOutA3b);
			ae_int32x2 vecOutA57 = AE_SEL32_HH(vecOutA5d, vecOutA7f);
			ae_int16x4 vecOutz = Packet(vecOutA02, vecOutA46, vecOutA13, vecOutA57);
			AE_S16X4_IP(vecOutz, outA, 8);
			ae_int32x2 vecOutA8a = AE_SEL32_LL(vecOutA08, vecOutA2a);
			ae_int32x2 vecOutAce = AE_SEL32_LL(vecOutA4c, vecOutA6e);
			ae_int32x2 vecOutA9b = AE_SEL32_LL(vecOutA19, vecOutA3b);
			ae_int32x2 vecOutAdf = AE_SEL32_LL(vecOutA5d, vecOutA7f);
			vecOutz = Packet(vecOutA8a, vecOutAce, vecOutA9b, vecOutAdf);
			AE_S16X4_IP(vecOutz, outA, 8);

			ae_int32x2 vecOutB08 = vecTmpY46 + vecTmpV23;
			ae_int32x2 vecOutB19 = vecTmpY46 + vecTmpUV23;
			ae_int32x2 vecOutB2a = vecTmpY46 + vecTmpU23;
			ae_int32x2 vecOutB3b = 0xff00;
			ae_int32x2 vecOutB4c = vecTmpY57 + vecTmpV23;
			ae_int32x2 vecOutB5d = vecTmpY57 + vecTmpUV23;
			ae_int32x2 vecOutB6e = vecTmpY57 + vecTmpU23;
			ae_int32x2 vecOutB7f = 0xff00;

			ae_int32x2 vecOutB02 = AE_SEL32_HH(vecOutB08, vecOutB2a);
			ae_int32x2 vecOutB46 = AE_SEL32_HH(vecOutB4c, vecOutB6e);
			ae_int32x2 vecOutB13 = AE_SEL32_HH(vecOutB19, vecOutB3b);
			ae_int32x2 vecOutB57 = AE_SEL32_HH(vecOutB5d, vecOutB7f);
			vecOutz = Packet(vecOutB02, vecOutB46, vecOutB13, vecOutB57);
			AE_S16X4_IP(vecOutz, outA, 8);
			ae_int32x2 vecOutB8a = AE_SEL32_LL(vecOutB08, vecOutB2a);
			ae_int32x2 vecOutBce = AE_SEL32_LL(vecOutB4c, vecOutB6e);
			ae_int32x2 vecOutB9b = AE_SEL32_LL(vecOutB19, vecOutB3b);
			ae_int32x2 vecOutBdf = AE_SEL32_LL(vecOutB5d, vecOutB7f);
			vecOutz = Packet(vecOutB8a, vecOutBce, vecOutB9b, vecOutBdf);
			AE_S16X4_IP(vecOutz, outA, 8);

			AE_L16X4_IP(vecInYA, inY2, 8);
			AE_L16X4_IP(vecInYB, inY2, 8);
			vecInYA = vecInYA - vecTmpC2;
			vecInYB = vecInYB - vecTmpC2;

			AE_MUL16X4(vecTmpY01, vecTmpY23, vecInYA, vecCoeY);
			AE_MUL16X4(vecTmpY45, vecTmpY67, vecInYB, vecCoeY);
			vecTmpY02 = AE_SEL32_HH(vecTmpY01, vecTmpY23);
			vecTmpY13 = AE_SEL32_LL(vecTmpY01, vecTmpY23);
			vecTmpY46 = AE_SEL32_HH(vecTmpY45, vecTmpY67);
			vecTmpY57 = AE_SEL32_LL(vecTmpY45, vecTmpY67);

			vecOutA08 = vecTmpY02 + vecTmpV01;
			vecOutA19 = vecTmpY02 + vecTmpUV01;
			vecOutA2a = vecTmpY02 + vecTmpU01;
			vecOutA3b = 0xff00;
			vecOutA4c = vecTmpY13 + vecTmpV01;
			vecOutA5d = vecTmpY13 + vecTmpUV01;
			vecOutA6e = vecTmpY13 + vecTmpU01;
			vecOutA7f = 0xff00;

			vecOutA02 = AE_SEL32_HH(vecOutA08, vecOutA2a);
			vecOutA46 = AE_SEL32_HH(vecOutA4c, vecOutA6e);
			vecOutA13 = AE_SEL32_HH(vecOutA19, vecOutA3b);
			vecOutA57 = AE_SEL32_HH(vecOutA5d, vecOutA7f);
			vecOutz = Packet(vecOutA02, vecOutA46, vecOutA13, vecOutA57);
			AE_S16X4_IP(vecOutz, outB, 8);

			vecOutA8a = AE_SEL32_LL(vecOutA08, vecOutA2a);
			vecOutAce = AE_SEL32_LL(vecOutA4c, vecOutA6e);
			vecOutA9b = AE_SEL32_LL(vecOutA19, vecOutA3b);
			vecOutAdf = AE_SEL32_LL(vecOutA5d, vecOutA7f);
			vecOutz = Packet(vecOutA8a, vecOutAce, vecOutA9b, vecOutAdf);
			AE_S16X4_IP(vecOutz, outB, 8);

			vecOutB08 = vecTmpY46 + vecTmpV23;
			vecOutB19 = vecTmpY46 + vecTmpUV23;
			vecOutB2a = vecTmpY46 + vecTmpU23;
			vecOutB3b = 0xff00;
			vecOutB4c = vecTmpY57 + vecTmpV23;
			vecOutB5d = vecTmpY57 + vecTmpUV23;
			vecOutB6e = vecTmpY57 + vecTmpU23;
			vecOutB7f = 0xff00;

			vecOutB02 = AE_SEL32_HH(vecOutB08, vecOutB2a);
			vecOutB46 = AE_SEL32_HH(vecOutB4c, vecOutB6e);
			vecOutB13 = AE_SEL32_HH(vecOutB19, vecOutB3b);
			vecOutB57 = AE_SEL32_HH(vecOutB5d, vecOutB7f);
			vecOutz = Packet(vecOutB02, vecOutB46, vecOutB13, vecOutB57);
			AE_S16X4_IP(vecOutz, outB, 8);

			vecOutB8a = AE_SEL32_LL(vecOutB08, vecOutB2a);
			vecOutBce = AE_SEL32_LL(vecOutB4c, vecOutB6e);
			vecOutB9b = AE_SEL32_LL(vecOutB19, vecOutB3b);
			vecOutBdf = AE_SEL32_LL(vecOutB5d, vecOutB7f);
			vecOutz = Packet(vecOutB8a, vecOutBce, vecOutB9b, vecOutBdf);
			AE_S16X4_IP(vecOutz, outB, 8);
		}
		inY1 += width_4;
		inY2 += width_4;
		outA += width_2;
		outB += width_2;
	}
	//ifrm->size -= total * 3;
	//ofrm->size = total << 2;
	return true;
}

EXTERNC void smf_i420p16s_rgba8888_register() {
	VideoFormats().Add(vfid32("yuv", i420p16s, "rgb", rgba8888), &convert);
}