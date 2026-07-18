#pragma once
#include "unique_ptr.h"

namespace smf {
	class JsonParser {
	public:
		enum class eType : uint8_t {
			null = 0,
			array,
			object,
			int_,
			bool_,
			float_,
			fraction,
			string,
		};
		enum class eCmd : uint8_t {
			null = 0,
			init,
			deinit,
			set,
			add,
		};
	public:
		class Node {
		public:
			friend class JsonParser;
		public:
			Node* _parent = 0;
			eType _type = eType::null;
			const char* _keys = 0;
			union {
				int32_t _value = 0;
				int32_t _int;
				bool _bool;
				float _float;
				const char* _string;
				void* _object;
			};
		public:
			Node(JsonParser*parser, Node*parent, eType type, const char* keys = 0);
			~Node();
		public:
			void Set(void*);
		public:
			const char* Keys()const;
			void* Object()const;
			void* ObjectParent()const;
			void Object(void*);
			int Level()const;
			void Path(char*)const;
		protected:
			void rePath(char*&)const;
		};
		friend class Node;
		typedef void (*CbUpdate)(void* priv, Node* node, eCmd cmd);
	public:
		JsonParser();
		~JsonParser();
	public:
		JsonParser(const JsonParser&) = delete;
		JsonParser& operator =(const JsonParser&) = delete;
	public:
		bool Parse(const char* str, int len = 0, bool nocopy = false);		
		void Register(CbUpdate cb, void* priv);
	protected:		
		bool ParseInt64(Node&);
		bool ParseDouble(Node&);
		bool ParseFraction(Node&);
		bool ParseString(Node&);
		bool ParseBool(Node&);
		bool ParseObject(Node&);
		bool ParseArray(Node&);
	protected:
		char* ParseString();
		bool ParseValue(Node*, const char* keys = 0);
	protected:
		eType TypeDetect();
		void SkipSpaces();
	protected:
		void NodeUpdate(Node& node, eCmd cmd);
		void Callback(Node* node, eCmd cmd);
	protected:
		VoidPtr _string;
		//const char* _source = 0;
		char* _begin = 0;
		char* _ptr = 0;
		char* _end = 0;
		//string _path;
	protected:
		void* _cbpriv = 0;
		CbUpdate _cb = 0;
	};
}

