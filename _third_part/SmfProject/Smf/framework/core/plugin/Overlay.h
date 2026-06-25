#pragma once
#include "Object.h"
#include "IMedia.h"
namespace smf {
	class Overlay :public Object {
	public:
		Overlay();
		virtual ~Overlay();
	protected:
		virtual bool set(uint32_t key, void* val)override;
		//virtual bool get(uint32_t key, void* val)const override;
	protected:
		uint32_t _tbl[16];
	protected:
		bool Add(uint32_t idx,uint32_t hash);
		bool Find(uint32_t& idx, uint32_t hash);
		bool Load(uint32_t);
		bool Load(const char*);
		bool Load(IMedia*);
		bool UnLoad();
	};
}
