#pragma once
#include <stdint.h>
namespace smf {
	class BsReader{
	protected:
		uint8_t* _begin = 0;
		uint8_t* _end = 0;
		uint32_t* _ptr = 0;
		int _bits = 0;
		uint32_t _cache = 0 ;
	public:
		BsReader() {}
		BsReader(void* buff, int size) { Init(buff, size); }
		void Init(void* buff, int size);
		unsigned Peak(int bits) const;
		unsigned Read(int bits);
		unsigned GetLeftBits() const;
		unsigned GetUsedBits() const;
	};

	class BsWriter{
	protected:
		uint8_t* _begin = 0;
		uint8_t* _end = 0;
		uint32_t* _ptr = 0;
		int _bits = 0;
		uint32_t _cache = 0;
	public:
		BsWriter() {}
		BsWriter(void* buff, int size) { Init(buff, size); }
		void Init(void* buff, int size);
		bool Write(uint32_t v,int bits);
		void Flush();
		unsigned GetLeftBits() const;
		unsigned GetUsedBits() const;
	};

	class ExponentialColumbianEntropy {
	public:
		static bool Encode(BsWriter& bs, uint32_t v);
		static uint32_t Decode(BsReader& bs);

		static bool Encode(BsWriter& bs, int32_t v);
		static int32_t Decodes(BsReader& bs);
	};
}