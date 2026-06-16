#pragma once
#include "ObjectStatic.h"
#include "Frame.h"
namespace smf {
	class IHook :public ObjectStatic {
	public:
		virtual ~IHook() {}
	public:
		virtual bool Hook(Frame*&) = 0;
	};
}