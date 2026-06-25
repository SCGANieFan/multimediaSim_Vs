#pragma once
#include "Element.h"
#include <stdint.h>
namespace smf{
	class SmfRecombine
		:public Object
	{
	public:
		SmfRecombine(Object* parent,int max);
		virtual ~SmfRecombine();
	protected:
		char* _buff = 0;
		int _size = 0;
		int _max = 0;		
		char* _fbuff = 0;
	public:
		bool Recombine(Frame* frm);
	};
}

