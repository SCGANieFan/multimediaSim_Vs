#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include "OggApi.h"

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


void* OggMalloc(int size) {
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

void* OggCalloc(int num, int size) {
	void* ptr = calloc(num, size);
	LOG_OGG("calloc, (%d,%d,%p)", num, size, ptr);
	return ptr;
}
void* OggRealloc(void* ptr, int size) {
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
	OggMuxerApiParam_t oggMuxerApiParam;

	oggMuxerApiParam.malloc_cb = OggMalloc;
	oggMuxerApiParam.calloc_cb = OggCalloc;
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
	OggRet_t ret;
	ret = OggMuxerApiCreate(&oggMuxerApiParam, &oggOpusMuxer);
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
		if (num == 818)
		{
			isLastData = true;
			int a = 1;
		}
		LOG_OGG("[%d] %d", num++, ifrm->size);
		}
#endif


		if (isLastData) {
			OggMuxerApiSet(oggOpusMuxer, OggMuxerApiSet_e::OGG_MUXER_API_SET_IS_EOS, (void*)1);
			ret = OggMuxerApiReceive(oggOpusMuxer, (uint8_t*)ifrm->buff, ifrm->size);
			if (ret != OGG_RET_SUCCESS) {
				LOG_OGG();
				goto exit;
			}
		}
		else {
			ret = OggMuxerApiReceive(oggOpusMuxer, (uint8_t*)ifrm->buff, ifrm->size);
			if (ret != OGG_RET_SUCCESS) {
				LOG_OGG();
				goto exit;
			}
		}

		ret = OggMuxerApiGenerate(oggOpusMuxer);
		if (ret == OGG_RET_SUCCESS) {
			OggPage_t oggPage;
			OggMuxerApiGet(oggOpusMuxer, OggMuxerApiGet_e::OGG_MUXER_API_GET_DATA_PAGE, &oggPage);
			fwrite(oggPage.headData, 1, oggPage.headLen, fOut);
			fwrite(oggPage.bodyData, 1, oggPage.bodyLen, fOut);
			LOG_OGG("%d", oggPage.headLen+ oggPage.bodyLen);
		}
	}
exit:
	OggMuxerApiDestory(oggOpusMuxer);
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
	oggDeMuxerApiParam.calloc_cb = OggCalloc;
	oggDeMuxerApiParam.realloc_cb = OggRealloc;
	oggDeMuxerApiParam.free_cb = OggFree;
	oggDeMuxerApiParam.printf_cb = OggPrint;
	ret = OggDeMuxerApiCreate(&oggDeMuxerApiParam, &oggDemuxer);
	bool idHeadIsOpus = false;
	while (1) {
		if (ifrm->offset) {
			memmove((int8_t*)ifrm->buff, (int8_t*)ifrm->buff + ifrm->offset, ifrm->size);
			ifrm->offset = 0;
		}
		OggDeMuxerApiReceiveInfo_t receiveInfo;
		OggDeMuxerApiGet(oggDemuxer, OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_RECEIVE_INFO, (void*)&receiveInfo);
		uint8_t* iBuff = (uint8_t*)ifrm->buff + ifrm->offset;
		int32_t remSize = ifrm->max - ifrm->offset - ifrm->size;
		int32_t readByte = fread(iBuff + ifrm->size, 1, remSize, fIn);
		//LOG_OGG("readByte:%d", readByte);
		if (readByte == 0) {
			break;
		}
		ifrm->size += readByte;
		//input
		int32_t copyByte = ifrm->size < receiveInfo.bufMax ? ifrm->size : receiveInfo.bufMax;
		memcpy(receiveInfo.buf, iBuff, copyByte);
		ret = OggDeMuxerApiReceive(oggDemuxer, copyByte);
		if (ret == OGG_RET_FAIL) {
			LOG_OGG(); return;
		}
		ifrm->offset += copyByte;
		ifrm->size -= copyByte;
		if (ret == OGG_RET_MORE_DATA) {
			continue;
		}

		uint32_t* hasPage = 0;
		OggDeMuxerApiGet(oggDemuxer, OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_HAS_ID_PAGE, (void*)&hasPage);
		if (hasPage &&!idHeadIsOpus) {
			OggPage_t* idPage = 0;
			OggDeMuxerApiGet(oggDemuxer, OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_ID_PAGE, (void*)&idPage);
			if (idPage) {
				//LOG_OGG("%.8s", idHead);
			}
			if (strcmp((char*)idPage->bodyData,"OpusHead")) {
				idHeadIsOpus = true;
			}

		}

		OggDeMuxerApiGet(oggDemuxer, OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_HAS_USER_PAGE, (void*)&hasPage);
		if (hasPage) {
			OggPage_t* userPage = 0;
			OggDeMuxerApiGet(oggDemuxer, OggDeMuxerApiGet_e::OGG_DEMUXER_API_GET_USER_PAGE, (void*)&userPage);
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
		if (num == 31)
			int a = 1;
		LOG_OGG("[%d]", num++); }
#endif
		while (1) {
			if (idHeadIsOpus) {
				oByte = ofrm->max - ofrm->offset - ofrm->size;
				ret = OggDeMuxerApiGenerate(oggDemuxer, oBuff + ofrm->size + 8, &oByte);
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
#if 1
				{static int num = 0;
				if (num == 802)
					int a = 1;
				LOG_OGG("[%d] %d", num++, oByte); }
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
				ret = OggDeMuxerApiGenerate(oggDemuxer, oBuff + ofrm->size, &oByte);
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
	OggDeMuxerApiDestory(oggDemuxer);
	free(ifrmOri.buff);
	free(ofrmOri.buff);
	fclose(fIn);
	fclose(fOut);
	return;
}



void OggTest() {
	LOG_OGG();
	//OggMux(SOURCE_PATH OGGENC_FILE_NAME);
	OggDemux(SOURCE_PATH OGGDEC_FILE_NAME);
}
