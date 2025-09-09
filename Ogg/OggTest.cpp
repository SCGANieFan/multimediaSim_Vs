#if 0
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include "ogg_api.h"

#define LOG_OGG(fmt,...)		printf("<%s>[%s](%d)" fmt "\n", strrchr(__FILE__,'\\') + 1, __func__, __LINE__, ##__VA_ARGS__)
#define SOURCE_PATH "../../source/container/ogg/"
#define OGGENC_FILE_NAME "test.opusx"
#define OGGDEC_FILE_NAME OGGENC_FILE_NAME ".ogg"
//#define OGGDEC_FILE_NAME OGGENC_FILE_NAME ".oggOri"

typedef struct
{
	void* buff;
	int offset;
	int size;
	int max;
}Frame;


void* OggMalloc(uint32_t size) {
	static int32_t sizeTotal = 0;
#if 1
	sizeTotal += size;
	void* ptr = malloc(size);
	LOG_OGG("malloc, ptr:%p, size:%d, sizeTotal:%d,", ptr, size, sizeTotal);
	return ptr;
#else
	return ((ALGO_Malloc_t)_malloc)(size);
#endif
}

void* OggRealloc(void* ptr, uint32_t size) {
	void *ptrNew = realloc(ptr, size);
	LOG_OGG("realloc, (%p->%p,%d)", ptr, ptrNew, size);
	return ptrNew;
}

static void OggFree(void* ptr) {
#if 1
	LOG_OGG("free, ptr:%p", ptr);
#endif
	return free(ptr);
}
static void OggPrint(const char* fmt, ...) {
	static char buf[256];
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	printf(buf);
}

void OggMux(const char* fileNameIn)
{
	char fileNameOut[512];
	sprintf(fileNameOut, "%s.ogg", fileNameIn);
	LOG_OGG("fin %s", fileNameIn);
	LOG_OGG("fout %s", fileNameOut);
	FILE* fIn = fopen(fileNameIn, "rb");
	FILE* fOut = fopen(fileNameOut, "wb");

	Frame ifrmOri;
	ifrmOri.max = 1 * 1024;
	ifrmOri.offset = 0;
	ifrmOri.size = 0;
	ifrmOri.buff = malloc(ifrmOri.max);
	Frame* ifrm = &ifrmOri;
	//init
	void* oggOpusMuxer;
	OggMuxerApiParam_t oggMuxerApiParam = {0};

	oggMuxerApiParam.malloc_cb = OggMalloc;
	oggMuxerApiParam.realloc_cb = OggRealloc;
	oggMuxerApiParam.free_cb = OggFree;
	oggMuxerApiParam.printf_cb = OggPrint;
	oggMuxerApiParam.mode = OggMuxerApiMode_e::OGG_MUXER_API_MODE_OPUS;
	oggMuxerApiParam.idParam.version = 1;
	oggMuxerApiParam.idParam.channel = 1;
	oggMuxerApiParam.idParam.preSkip = 312;
	oggMuxerApiParam.idParam.sampleRate = 16000;
	oggMuxerApiParam.idParam.outPutGain = 0;
	uint8_t vendorString[] = "Lavf60.16.100";
	uint8_t userComment[] = "encoder=Lavc60.31.102 libopus";
	oggMuxerApiParam.userComment.vendorString = vendorString;
	oggMuxerApiParam.userComment.vendorStringLen = sizeof(vendorString) - 1;
	oggMuxerApiParam.userComment.userCommentString = userComment;
	oggMuxerApiParam.userComment.userCommentStringLen = sizeof(userComment) - 1;
	//oggMuxerApiParam.page_byte_round = 400;
	OggRet_t ret;
	ret = ogg_muxer_api_create(&oggMuxerApiParam, &oggOpusMuxer);
	if (ret != OGG_RET_SUCCESS) {
		LOG_OGG();
	}

	bool isLastData = false;
	while (1) {
		int32_t headMax=8;
		uint8_t head[8];
		int32_t readByte = fread(head, 1, headMax, fIn);
		if (readByte < headMax) {
			goto exit;
		}
		int32_t len = ((int32_t)head[0] << 24) | (int32_t)head[1] << 16 | (int32_t)head[2] << 8 | head[3];
		readByte = fread(ifrm->buff, 1, len, fIn);
		if (readByte < len) {
			goto exit;
		}
		ifrm->size = len;
#if 1
		{static int num = 0;
		static int acc = 0;
		if (num == 818)
		{
			isLastData = true;
			int a = 1;
		}
		acc += ifrm->size;
		//LOG_OGG("[%d] %d,%d", num++, ifrm->size, acc);
		}
#endif


		if (isLastData) {
			ogg_muxer_api_set(oggOpusMuxer, OggMuxerApiSet_e::OGG_MUXER_API_SET_IS_EOS, (void*)1);
			ret = ogg_muxer_api_receive(oggOpusMuxer, (uint8_t*)ifrm->buff, ifrm->size);
			if (ret != OGG_RET_SUCCESS) {
				LOG_OGG();
				goto exit;
			}
		}
		else {
			ret = ogg_muxer_api_receive(oggOpusMuxer, (uint8_t*)ifrm->buff, ifrm->size);
			if (ret != OGG_RET_SUCCESS) {
				LOG_OGG();
				goto exit;
			}
		}

		ret = ogg_muxer_api_generate(oggOpusMuxer);
		if (ret == OGG_RET_SUCCESS) {
			OggPage_t oggPage;
			ogg_muxer_api_get(oggOpusMuxer, OggMuxerApiGet_e::OGG_MUXER_API_GET_DATA_PAGE, &oggPage);
			fwrite(oggPage.headData, 1, oggPage.headLen, fOut);
			fwrite(oggPage.bodyData, 1, oggPage.bodyLen, fOut);
			LOG_OGG("%d", oggPage.headLen+ oggPage.bodyLen);
		}
	}
exit:
	ogg_muxer_api_destory(oggOpusMuxer);
	free(ifrmOri.buff);
	fclose(fIn);
	fclose(fOut);
	return;
}

void OggDemux(const char* fileNameIn)
{
	char fileNameOut[512];
	sprintf(fileNameOut, "%s.demux", fileNameIn);
	LOG_OGG("fin %s", fileNameIn);
	LOG_OGG("fout %s", fileNameOut);
	FILE* fIn = fopen(fileNameIn, "rb");
	FILE* fOut = fopen(fileNameOut, "wb");

	Frame ifrmOri;
	Frame ofrmOri;
	ifrmOri.max = 1 * 1024;
	ifrmOri.offset = 0;
	ifrmOri.size = 0;
	ifrmOri.buff = malloc(ifrmOri.max);
	ofrmOri.max = 10 * 1024;
	ofrmOri.offset = 0;
	ofrmOri.size = 0;
	ofrmOri.buff = malloc(ofrmOri.max);
	Frame* ifrm = &ifrmOri;
	Frame* ofrm = &ofrmOri;

	//init
	OggRet_t ret;
	void* oggDemuxer = 0;
	OggDeMuxerApiParam_t oggDeMuxerApiParam;
	oggDeMuxerApiParam.malloc_cb = OggMalloc;
	oggDeMuxerApiParam.realloc_cb = OggRealloc;
	oggDeMuxerApiParam.free_cb = OggFree;
	oggDeMuxerApiParam.printf_cb = OggPrint;
	ret = ogg_demuxer_api_create(&oggDeMuxerApiParam, &oggDemuxer);
	bool idHeadIsOpus = false;
	while (1) {
		if (ifrm->offset) {
			memmove((int8_t*)ifrm->buff, (int8_t*)ifrm->buff + ifrm->offset, ifrm->size);
			ifrm->offset = 0;
		}
		OggDeMuxerApiReceiveInfo_t receiveInfo;
		ogg_demuxer_api_get(oggDemuxer, OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_RECEIVE_INFO, (void*)&receiveInfo);
		uint8_t* iBuff = (uint8_t*)ifrm->buff + ifrm->offset;
		int32_t remSize = ifrm->max - ifrm->offset - ifrm->size;
		int32_t readByte = fread(iBuff + ifrm->size, 1, remSize, fIn);
		static int readByteAcc = 0;
		readByteAcc += readByte;
		LOG_OGG("readByte:%d,%d", readByte, readByteAcc);
		if (readByte == 0) {
			break;
		}
		ifrm->size += readByte;
		//input
		int32_t copyByte = ifrm->size < receiveInfo.bufMax ? ifrm->size : receiveInfo.bufMax;
		memcpy(receiveInfo.buf, iBuff, copyByte);
		ret = ogg_demuxer_api_receive(oggDemuxer, copyByte);
		if (ret == OGG_RET_FAIL) {
			LOG_OGG(); return;
		}
		ifrm->offset += copyByte;
		ifrm->size -= copyByte;
		if (ret == OGG_RET_MORE_DATA) {
			continue;
		}

		uint32_t* hasPage = 0;
		ogg_demuxer_api_get(oggDemuxer, OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_HAS_ID_PAGE, (void*)&hasPage);
		if (hasPage &&!idHeadIsOpus) {
			OggPage_t* idPage = 0;
			ogg_demuxer_api_get(oggDemuxer, OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_ID_PAGE, (void*)&idPage);
			if (idPage) {
				//LOG_OGG("%.8s", idHead);
			}
			if (strcmp((char*)idPage->bodyData,"OpusHead")) {
				idHeadIsOpus = true;
			}

		}

		ogg_demuxer_api_get(oggDemuxer, OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_HAS_USER_PAGE, (void*)&hasPage);
		if (hasPage) {
			OggPage_t* userPage = 0;
			ogg_demuxer_api_get(oggDemuxer, OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_USER_PAGE, (void*)&userPage);
			if (userPage) {
				int a = 1;
			}
		}
		/* the other page body*/
		uint8_t* oBuff = (uint8_t*)ofrm->buff + ofrm->offset;
		int32_t oBuffLeft = ofrm->max - ofrm->offset - ofrm->size;
		int32_t oByte;
#if 1
		{static int num = 0;
		if (num == 3)
			int a = 1;
		LOG_OGG("[%d]", num++); }
#endif
		while (1) {
			if (idHeadIsOpus) {
				oByte = ofrm->max - ofrm->offset - ofrm->size;
				ret = ogg_demuxer_api_generate(oggDemuxer, oBuff + ofrm->size + 8, &oByte);
				if (ret == OGG_RET_FAIL) {
					LOG_OGG();
					goto exit;
				}
				else if (ret == OGG_RET_MORE_DATA) {
					break;
				}
				else  if (ret == OGG_RET_OUT_BUFF_NOT_ENOUGH) {
					LOG_OGG();
				}
				else if (!oByte)
					break;
				if (!oByte) {
					int a = 1;
				}
#if 1
				{static int num = 0;
				static int acc = 0;
				if (num == 19)
					int a = 1;
				acc += oByte;
				LOG_OGG("[%d] %d,%d", num++, oByte, acc); }
#endif
#if 1
				char B4[4];
				//pack size, 4byte
				B4[3] = oByte & 0xff;
				B4[2] = (oByte >> 8) & 0xff;
				B4[1] = (oByte >> 16) & 0xff;
				B4[0] = (oByte >> 24) & 0xff;
				memcpy(oBuff + ofrm->size, B4, 4);
				oByte += 4;
#if 1
				//range code, 4byte
				B4[0] = 0;
				B4[1] = 0;
				B4[2] = 0;
				B4[3] = 0;
				memcpy(oBuff + ofrm->size + 4, B4, 4);
				oByte += 4;
#endif
#endif
			}
			else {
				oByte = ofrm->max - ofrm->offset - ofrm->size;
				ret = ogg_demuxer_api_generate(oggDemuxer, oBuff + ofrm->size, &oByte);
				if (ret == OGG_RET_FAIL) {
					LOG_OGG();
					goto exit;
				}
				else if (ret == OGG_RET_MORE_DATA) {
					break;
				}
				else  if (ret == OGG_RET_OUT_BUFF_NOT_ENOUGH) {
					LOG_OGG();
				}
				else if (!oByte)
					break;
			}
			ofrm->size += oByte;
		}

		fwrite(oBuff, 1, ofrm->size, fOut);
		ofrm->offset = 0;
		ofrm->size = 0;
	}
exit:
	ogg_demuxer_api_destory(oggDemuxer);
	free(ifrmOri.buff);
	free(ofrmOri.buff);
	fclose(fIn);
	fclose(fOut);
	return;
}



void OggTest() {
	LOG_OGG();
	OggMux(SOURCE_PATH OGGENC_FILE_NAME);
	//OggDemux(SOURCE_PATH OGGDEC_FILE_NAME);
}
#endif


#include"MTF.h"
using namespace MTFApi_ns;

#define PATH "../../source/container/ogg/"
#define FILE_NAME "test.opusx"
//#define FILE_NAME "mbz_48k2h_40s.opusx"
//#define FILE_NAME "test48k2ch.ogg"

#define RATE 16000
#define CHANNEL 1
#define WIDTH 2

using namespace MTFApi_ns;




void OggTest()
{
	MTFApi::Init();
	MTF_REGISTER(opus_demuxer);
	MTF_REGISTER(opus_muxer);
	MTF_REGISTER(ogg_demuxer);
	MTF_REGISTER(ogg_muxer);
#if 1
	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".ogg"),
		(void*)(2048),
		(void*)(RATE),
		(void*)(CHANNEL),
		(void*)(WIDTH),
	};
	//, url = $0, fSamples = $2
	const char* str = {
	"|opus_demuxer,url=$0,rate=$3,ch=$4,witdh=$5|-->"
	"|ogg_muxer,url=$1,pagebyte=$2|"
	};
#else
	void* param[] = {
	(void*)(PATH FILE_NAME),
	(void*)(PATH FILE_NAME ".ogg"),
	(void*)(2048),
	(void*)(RATE),
	(void*)(CHANNEL),
	(void*)(WIDTH),
	};
	//, url = $0, fSamples = $2
	const char* str = {
	"|ogg_demuxer,url=$0,rate=$3,ch=$4,witdh=$5|-->"
	"|ogg_muxer,url=$1,pagebyte=$2|"
	};
#endif
	MTFApi::Api(str, param);

}



