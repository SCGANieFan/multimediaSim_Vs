#pragma once
#include"MTF.Type.h"
#include"MTF.MemoryBuffNode.h"
#include"MTF.Memory_c.h"
namespace mtf_ns {

class MTF_MemoryBuff_c:public MTF_IMemory_c
{
public:
	MTF_MemoryBuff_c();
    MTF_MemoryBuff_c(void* buff,int size, int thr = 0);
	~MTF_MemoryBuff_c();
public:
    static void* operator new(size_t size, void* ptr) { return ptr; }
public:
    bool Initialize(void* buff,int size,int thr=0);
    void Uninit();
    bool Reset();
	public:
		bool IsValid()const;
		bool IsValid(void*)const;
		operator bool()const;
		char* Print(char* ptr, char* end) const;
		void Print() const;
		void PrintError() const;
	public:
#if 1
		virtual mtf_void* Malloc(mtf_i32 size) override;
		virtual mtf_void* Realloc(mtf_void* block, mtf_i32 size) override;
		virtual mtf_void* Calloc(mtf_i32 count, mtf_i32 size) override;
		virtual mtf_void Free(mtf_void* block) override;
#else
		void* Malloc(unsigned size, unsigned align = 0);
		void* Realloc(void* ptr, unsigned size);
		void* Calloc(unsigned num, unsigned size);
		bool Free(void* ptr);
#endif
		bool Hold(void* ptr);
		uint32_t GetFreeSize() const;
		uint32_t GetFreeNodeCount() const;
		uint32_t GetUsedSize() const;
		uint32_t GetUsedNodeCount() const;
		uint32_t GetTotalSize() const;
		uint32_t GetTotalNodeCount() const;
		uint32_t GetSize(void*ptr) const;
		uint32_t GetRefs(void*ptr) const;
		void SetThreshold(uint32_t);
		void* GetBegin()const;
		void* GetEnd()const;
	public:
		using Node = MTF_MemoryBuffNode_c;
	protected:
		uint8_t* _buff = 0;
		uint32_t _buffSize = 0;
		uint32_t _threshold = 0;
	protected:
		Node* _node0 = 0;
		Node* _node1 = 0;
		uint32_t _total = 0;
		uint32_t _free = 0;
		uint32_t _totalCount = 0;
		uint32_t _freeCount = 0;
	protected:
		bool free(Node* node);
		bool merge0(Node* node);
		bool merge1(Node* node);
		bool move(Node* dst, Node* src)const;
		Node* insert(Node* node,unsigned index);
		//Node* insert1(Node* node,unsigned index);
		Node* alloc(unsigned size);
		Node* alloc(unsigned size, unsigned align);
		Node* alloc0(Node* node, unsigned index);
		Node* alloc1(Node* node, unsigned index);
		//Node* alloc1(Node* node, unsigned index);
		Node* alloc0(unsigned size);
		Node* alloc1(unsigned size);
		Node* realloc(void* ptr, unsigned size);
		Node* realloc(Node* node, unsigned size);
		Node* find0(void* ptr)const;
		Node* find1(void* ptr)const;
		Node* findBadNode0(uint32_t& cnt)const;
		Node* findBadNode1(uint32_t& cnt)const;
		Node* repair()const;
		Node* getNode(void*)const;
	public:
		static MTF_MemoryBuff_c* Create(void* buff, int size, int thr = 0);
		static void Destroy(MTF_MemoryBuff_c*);
};

}


