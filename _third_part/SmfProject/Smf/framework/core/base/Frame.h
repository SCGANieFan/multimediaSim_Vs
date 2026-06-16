#pragma once
#include "smf_frame.h"
#include "ObjectStatic.h"
#include "IMedia.h"
#include "IPool.h"
namespace smf {
	class Port;
	class Frame 
		:public ObjectStatic 
		,public smf_frame_t
	{
	public:
		Frame();
		~Frame();
	public:
		IPool* Pool()const { return (IPool*)pool; }
		void Pool(IPool* pool_) { pool = pool_; }
		smf::Port* Port()const { return (smf::Port*)port; }
		void Port(smf::Port*port_) { port = port_; }
		IMedia* Media()const { return (IMedia*)media; }
		void Media(IMedia* media_) { media = (smf_media_def_t*)media_; }
		Frame*& Next() { return (Frame*&)frame; }
		int Left() { return max - offset - size; }
		void* LeftData() { return (char*)buff + offset + size; }
		void* Data() { return (char*)buff + offset; }
	public:
		uint64_t TimeStampBase() const { return sinfo ? sinfo->forward.timestamp : 0;}
		uint64_t TimeStamp() const { return timestamp + TimeStampBase(); }
		uint32_t TimeStampOffset() const { return timestamp; }
		void TimeStampReset();
		void TimeStampUpdate();
		void TimeStampBase(uint64_t tp);
		void TimeStamp(uint64_t tp) { timestamp = (uint32_t)(tp - TimeStampBase()); }
		void TimeStampOffset(uint32_t tp) { timestamp = tp; }
		void SetIndex(uint32_t idx);
		uint32_t NextIndex();
	public:
		uint64_t Codec() { return media ? media->_codec : 0; }
		const char* CodecString() { return media ? Media()->CodecString() : 0; }
		const char* CodecStringX() { return media ? Media()->CodecString() : ""; }
	public:
		void Hold() const { if (pool && buff)((IPool*)pool)->Hold((void*)buff); }
		void Free() const { if (pool && buff)((IPool*)pool)->Free((void*&)buff); }
	public:
		void CleanOffset();
	public:
		char* Print(char* ptr, char* end, bool timestamp = false, bool media = false);
	};
}
