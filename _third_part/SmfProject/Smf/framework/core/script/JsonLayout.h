#pragma once
#include "JsonParser.h"
#include "Object.h"
#include "SmfBuffer.h"
namespace smf {
	class JsonLayout: protected JsonParser {
	public:
		JsonLayout();
		~JsonLayout();
	protected:
		uint32_t* _params = 0;
		Object* _obj = 0;
		const char* _name = 0;
		const char* _type = 0;
		char* _string = 0;
	public:
		Object* Layout(const char* script, int len = 0, uint32_t* params = 0, bool nocopy = false);
	protected:
		void Callback(Node* node, eCmd cmd);
		void cbInit(Node* node);
		void cbDeinit(Node* node);
		void cbSet(Node* node);
	protected:
		bool Create(Node*);
		bool Find(Node*);
		bool Link(Node*, const char* dst);
		bool Set(Node*, const char* key, void* val, char chr = 0);
		const char* ParseString(const char*);
	};
}