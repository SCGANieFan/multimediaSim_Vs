#pragma once
#include "Source.h"
#include "IO.h"
#include "URL.h"
#include "smf_demuxer.h"
#include "unique_ptr.h"
#include "SmfBuffer.h"
namespace smf {
	class IOSource
		: public Source
	{
	public:
		IOSource();
		virtual ~IOSource();
	protected:
		virtual bool open(void*)override;
		virtual bool close()override;
		//virtual bool generateFrame(Frame*&)override;
		virtual bool setParam(void*) override;
		virtual bool set(uint32_t key, void* val) override;
		virtual bool get(uint32_t key, void* val) const override;
		virtual bool seekToIndex(uint32_t frameindex);
		virtual bool seekToMs(uint32_t timepoint);
		virtual bool processError(Frame* ifrm, Frame* ofrm)override;
		virtual char* print(char* ptr, char* end) const override;
	public:
		bool SeekTo(uint32_t frameindex);
		bool SeekFor(int32_t frameindex);
		bool SeekTo(const timepoint32_t& tp);
		bool SeekFor(const duration32_t& offset);
		bool SeekTo(const percentage_t& percent);
		bool SeekFor(const percentage_t& percent);
	protected:
		bool initIO();
		bool initExtra(const char* url);
		bool init(uint32_t framecount, uint32_t duration_ms);
		bool update(Frame*);
		bool fill(smf_meta_info_t* meta)const;
	protected:
		enum class eStart {
			null = 0,
			position,
			timepoint,
			index,
			percent
		};
		union {
			uint32_t val = 0;
			uint32_t position;
			timepoint32_t timepoint;
			uint32_t index;
			percentage_t percent;
		}_start;
		eStart _eStart = eStart::null;
		uint32_t _seekMs = 0xffffffff;
		uint32_t _seekIndex = 0xffffffff;
	public:
		void SetStartPosition(uint32_t v);
		void SetStartTimepoint(uint32_t v);
		void SetStartIndex(uint32_t v);
		void SetStartPercent(uint32_t v);
	protected:
		URL _url;
		int _ioCacheLen = 1024;
		int _ioCacheAlign = 512;
		IO* _io = 0;
	protected://old process
		smf_progress_t* _progress = 0;
	protected:
		smf_meta_info_t _metaX;
		smf_meta_info_t* _meta = &_metaX;
		region_t _timepoint;
		region_t _frameindex;
	protected:
		VoidBuffer _extra;
		VoidBuffer _title;
		VoidBuffer _artist;
		VoidBuffer _album;
	protected:
		enum {
			eLoopPlay = 1u << 0,
		};
	};

	//fixed frame size
	class IOSourceFixedFrame
		: public IOSource
	{
	protected:
		virtual bool open(void*)override;
		virtual bool generateFrame(Frame*&)override;
		virtual bool seekToIndex(uint32_t frameindex) override;
		virtual char* print(char* ptr, char* end) const override;
	protected:
		virtual bool parseFile() = 0;
	protected:
		uint32_t _hsize = 0;
		uint32_t _fsize = 0;
		uint32_t _fdura = 0;//us
	};

	//scan frame
	class IOSourceScanFrame
		: public IOSource
	{
	protected:
		virtual bool open(void*)override;
		virtual bool generateFrame(Frame*&)override;
		virtual bool seekToIndex(uint32_t frameindex) override;
		virtual char* print(char* ptr, char* end) const override;
	protected:
		virtual uint32_t parseFrame(void*data,int max) = 0;
		virtual bool parseFile() = 0;
	protected:
		bool scanFile();
	protected:
		uint32_t _hsize = 0;
		uint32_t _fcnt = 0;
		uint32_t _fdura = 0;//us
	};

	class IOSourceNormal
		: public IOSourceFixedFrame
	{
	protected:
		virtual bool set(uint32_t key, void* val) override;
		virtual bool parseFile() override;
	};

	template<class T>
	class TIOSourceNormal
		: public IOSourceNormal
		, public T
	{
	public:
		TIOSourceNormal() {
			this->InitMedia(this);
		}
	};
}
