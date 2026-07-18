#pragma once
#include "JsonParser.h"
#include "string.hh"

namespace smf {
	class JsonConvert: protected JsonParser {
	public:
		JsonConvert();
		~JsonConvert();
	protected:
		string _buff;
		char* _ptr = 0;
		char* _begin = 0;
		char* _end = 0;
	public:
		const char* Convert(const char* str);
	protected:
		void Callback(Node* node, eCmd cmd);
		void cbInit(Node* node);
		void cbDeinit(Node* node);
		void cbSet(Node* node);
	};
}