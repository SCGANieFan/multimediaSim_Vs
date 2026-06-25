#pragma once
#include "AutoElement.h"
#include "Filter.h"
namespace smf {
	class AutoFilter
		:public Filter
	{
	protected:
		AutoElement _ele{ this };
	protected:
		virtual bool set(uint32_t key, void* val)override;
		virtual bool open(void*)override;
	};
}