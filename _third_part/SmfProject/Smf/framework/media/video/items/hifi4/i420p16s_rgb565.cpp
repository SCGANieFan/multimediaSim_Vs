#include "VideoFormatList.h"
#include <xtensa/tie/xt_hifi2.h>
using namespace smf;
using namespace smf::video;

static inline ae_int16x4 Packet_16_565(ae_int16x4 aa, ae_int16x4 bb, ae_int16x4 cc) {
	ae_int16x4 na = 0xf800;
	ae_int16x4 nb = 0x7e0;
	ae_int16x4 nc = 0x1f;
	aa = aa & na;
	bb = bb >> 5;
	bb = bb & nb;
	cc = cc >> 11;
	cc = cc & nc;
	ae_int16x4 ret = aa | bb | cc;
	return ret;
}
//637124
static bool convert(VideoFormat* vf, void* idata, void*& odata) {
	// unsigned ticks = get_ticks();
	unsigned height = vf->_ih;
	unsigned height_2 = height >> 1;
	unsigned width = vf->_iw;
	unsigned width_4 = width >> 2;
	unsigned width_8 = width >> 3;
	unsigned widthN2 = width << 1;
	unsigned total = height * width;
	ae_int16x4* inY1 = (ae_int16x4*)idata;
	ae_int16x4* inY2 = inY1 + width_4;
	ae_int16x4* inU = inY1 + (total >> 2);
	ae_int16x4* inV = inU + (total >> 4);
	uint8_t* outZA = (uint8_t*)odata;
	uint8_t* outZB = (uint8_t*)odata + widthN2;
	ae_int16x4* outA = (ae_int16x4*)outZA;
	ae_int16x4* outB = (ae_int16x4*)outZB;
	ae_int16x4 vecInYA, vecInYB, vecInU, vecInV;
	ae_int16x4 vecTmpC1 = 16;
	ae_int16x4 vecTmpC2 = -32768;
	ae_int16x4 vecCoeY = 9535;//q13
	ae_int16x4 vecCoeU_R = 13074;//q13
	ae_int16x4 vecCoeU_G = -6660;//q13
	ae_int16x4 vecCoeV_G = -3211;//q13
	ae_int16x4 vecCoeV_B = 16523;//q13
	for (int i = 0; i < height_2; i++) {
		for (int j = 0; j < width_8; j++) {
			AE_L16X4_IP(vecInU, inU, 8); vecInU = AE_SLAA16S(vecInU, 8);//q8
			AE_L16X4_IP(vecInV, inV, 8); vecInV = AE_SLAA16S(vecInV, 8);//q8
			ae_int16x4 vecTmpU0123 = AE_MULFP16X4S(vecInU, vecCoeU_R);//q6
			ae_int16x4 vecTmpUV0123 = AE_MULFP16X4S(vecInU, vecCoeU_G) +
				AE_MULFP16X4S(vecInV, vecCoeV_G);
			ae_int16x4 vecTmpV0123 = AE_MULFP16X4S(vecInV, vecCoeV_B);
			ae_int16x4 vecTmpV0011 = AE_SEL16_7362(vecTmpV0123, vecTmpV0123);
			ae_int16x4 vecTmpV2233 = AE_SEL16_5140(vecTmpV0123, vecTmpV0123);
			ae_int16x4 vecTmpU0011 = AE_SEL16_7362(vecTmpU0123, vecTmpU0123);
			ae_int16x4 vecTmpU2233 = AE_SEL16_5140(vecTmpU0123, vecTmpU0123);
			ae_int16x4 vecTmpUV0011 = AE_SEL16_7362(vecTmpUV0123, vecTmpUV0123);
			ae_int16x4 vecTmpUV2233 = AE_SEL16_5140(vecTmpUV0123, vecTmpUV0123);
			AE_L16X4_IP(vecInYA, inY1, 8);
			AE_L16X4_IP(vecInYB, inY1, 8);
			vecInYA = vecInYA - vecTmpC1; vecInYA = AE_SLAA16S(vecInYA, 8);
			vecInYB = vecInYB - vecTmpC1; vecInYB = AE_SLAA16S(vecInYB, 8);
			ae_int16x4 vecTmpY0123 = AE_MULFP16X4S(vecInYA, vecCoeY);
			ae_int16x4 vecTmpY4567 = AE_MULFP16X4S(vecInYB, vecCoeY);
			ae_int16x4 vecOutB0123 = vecTmpY0123 + vecTmpV0011;
			ae_int16x4 vecOutB4567 = vecTmpY4567 + vecTmpV2233;
			ae_int16x4 vecOutG0123 = vecTmpY0123 + vecTmpUV0011;
			ae_int16x4 vecOutG4567 = vecTmpY4567 + vecTmpUV2233;
			ae_int16x4 vecOutR0123 = vecTmpY0123 + vecTmpU0011;
			ae_int16x4 vecOutR4567 = vecTmpY4567 + vecTmpU2233;
			vecOutB0123 = AE_SLAA16S(vecOutB0123, 2); vecOutB0123 += vecTmpC2;
			vecOutB4567 = AE_SLAA16S(vecOutB4567, 2); vecOutB4567 += vecTmpC2;
			vecOutG0123 = AE_SLAA16S(vecOutG0123, 2); vecOutG0123 += vecTmpC2;
			vecOutG4567 = AE_SLAA16S(vecOutG4567, 2); vecOutG4567 += vecTmpC2;
			vecOutR0123 = AE_SLAA16S(vecOutR0123, 2); vecOutR0123 += vecTmpC2;
			vecOutR4567 = AE_SLAA16S(vecOutR4567, 2); vecOutR4567 += vecTmpC2;
			ae_int16x4 vecOutZ = Packet_16_565(vecOutB0123, vecOutG0123, vecOutR0123);
			AE_S16X4_IP(vecOutZ, outA, 8);
			vecOutZ = Packet_16_565(vecOutB4567, vecOutG4567, vecOutR4567);
			AE_S16X4_IP(vecOutZ, outA, 8);
			AE_L16X4_IP(vecInYA, inY2, 8);
			AE_L16X4_IP(vecInYB, inY2, 8);
			vecInYA = vecInYA - vecTmpC1; vecInYA = AE_SLAA16S(vecInYA, 8);
			vecInYB = vecInYB - vecTmpC1; vecInYB = AE_SLAA16S(vecInYB, 8);
			vecTmpY0123 = AE_MULFP16X4S(vecInYA, vecCoeY);
			vecTmpY4567 = AE_MULFP16X4S(vecInYB, vecCoeY);
			vecOutB0123 = vecTmpY0123 + vecTmpV0011;
			vecOutB4567 = vecTmpY4567 + vecTmpV2233;
			vecOutG0123 = vecTmpY0123 + vecTmpUV0011;
			vecOutG4567 = vecTmpY4567 + vecTmpUV2233;
			vecOutR0123 = vecTmpY0123 + vecTmpU0011;
			vecOutR4567 = vecTmpY4567 + vecTmpU2233;
			vecOutB0123 = AE_SLAA16S(vecOutB0123, 2); vecOutB0123 += vecTmpC2;
			vecOutB4567 = AE_SLAA16S(vecOutB4567, 2); vecOutB4567 += vecTmpC2;
			vecOutG0123 = AE_SLAA16S(vecOutG0123, 2); vecOutG0123 += vecTmpC2;
			vecOutG4567 = AE_SLAA16S(vecOutG4567, 2); vecOutG4567 += vecTmpC2;
			vecOutR0123 = AE_SLAA16S(vecOutR0123, 2); vecOutR0123 += vecTmpC2;
			vecOutR4567 = AE_SLAA16S(vecOutR4567, 2); vecOutR4567 += vecTmpC2;
			vecOutZ = Packet_16_565(vecOutB0123, vecOutG0123, vecOutR0123);
			AE_S16X4_IP(vecOutZ, outB, 8);
			vecOutZ = Packet_16_565(vecOutB4567, vecOutG4567, vecOutR4567);
			AE_S16X4_IP(vecOutZ, outB, 8);
		}
		inY1 += width_4;
		inY2 += width_4;
		outA += width_4;
		outB += width_4;
	}
	//ifrm->size -= total * 3;
	//ofrm->size = total << 1;
	// unsigned ticks2 = get_ticks();
	// dbgTestPDL(ticks);
	// dbgTestPDL(ticks2);
	// unsigned sub = ticks2-ticks;
	// dbgTestPDL(sub);
	// dbgTestDump(ofrm->buff, 64);
	return true;
}

EXTERNC void smf_i420p16s_rgb565_register() {
	VideoFormats().Add(vfid32("yuv", i420p16s, "rgb", rgb565), &convert);
}