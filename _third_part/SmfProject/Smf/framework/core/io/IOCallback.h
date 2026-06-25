#pragma once
#include "IO.h"
#include <stdio.h>
#include "smf_api_param.h"
namespace smf {
	class IOCallback :public IO {
	public:
		typedef void* (*FOpen)(const char* __filename, const char* __modes);
		typedef int (*FClose)(void* __stream);
		typedef size_t(*FRead)(void* __ptr, size_t __size, size_t __n, void* __stream);
		typedef int (*FSeek)(void* __stream, long int __off, int __whence);
		typedef size_t(*FWrite)(const void* ptr, size_t size, size_t nmemb, void* __stream);
		typedef long (*FTell)(void* __stream);
		typedef long (*FLength)(void* __stream);
		typedef int (*FEof)(void* __stream);
		typedef int (*FError)(void* __stream);
		class callback_t: public api::SmfBase{
		public:
			FOpen _fopen = 0;
			FClose _fclose = 0;
			FRead _fread = 0;			
			FWrite _fwrite = 0;
			FSeek _fseek = 0;
			FTell _ftell = 0;
			FLength _flength = 0;
		};
	public:
		IOCallback() {}
		IOCallback(FOpen fopen_
			, FClose fclose_
			, FRead fread_
			, FWrite fwrite_
			, FSeek fseek_ = 0
			, FTell ftell_ = 0
			, FLength flength_ = 0			
			//, FEof feof_ = 0
			//, FError ferror_ = 0
		);
		virtual ~IOCallback();
	protected:
		void* _hd = 0;
	protected:
		FOpen _fopen = 0;
		FClose _fclose = 0;
		FRead _fread = 0;
		FSeek _fseek = 0;
		FWrite _fwrite = 0;
		FTell _ftell = 0;
		FLength _flength = 0;
		//FEof _feof = 0;
		//FError _ferror = 0;

		//unsigned _ticks = 0;
		//unsigned _tickTotal = 0;
	protected:
		virtual void initialize(Reflection::item_t*)override;
	public:
		typedef smf_io_param_t OpenParam;
	public:///status ctrl
		virtual bool Open(void*)override;
		virtual bool Close()override;
	public:///status ctrl
		virtual bool Seek(int offset, Position pos)override;
		virtual unsigned GetSize() const override;
		virtual unsigned GetOffset() const override;
		virtual unsigned Read(void* buff, unsigned size) override;
		virtual unsigned Write(void* buff, unsigned size) override;
	};
}

