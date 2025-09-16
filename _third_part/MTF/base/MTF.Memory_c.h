#pragma once
#include"MTF.Type.h"
namespace mtf_ns {
    typedef mtf_void* (*Malloc_cb)(int32_t size);
    typedef mtf_void* (*Realloc_cb)(mtf_void* block, int32_t size);
    typedef mtf_void* (*Calloc_cb)(int32_t count, int32_t size);
    typedef mtf_void(*Free_cb)(mtf_void* block);

    class MTF_IMemory_c
    {
    public:
        MTF_IMemory_c() {}
        ~MTF_IMemory_c() {}
    public:
        void* operator new(size_t size, void* ptr) {
            return ptr;
        }
    public:
        virtual void* Malloc(mtf_i32 size) = 0;
        virtual void* Realloc(mtf_void* block, mtf_i32 size) = 0;
        virtual void* Calloc(mtf_i32 count, mtf_i32 size) = 0;
        virtual void Free(mtf_void* block) = 0;
    };

    class MTF_Memory_c {
    public:
        class Item {
        public:
            Item() {}
            ~Item() {}
        public:
            const char* _name;
            mtf_u32 _id;
            mtf_u32 _type;
            MTF_IMemory_c* _memory;
        };
    public:
        MTF_Memory_c() {}
        MTF_Memory_c(Item* items, mtf_i32 numMax) {
            _items = items;
            _numMax = numMax;
        }
        ~MTF_Memory_c() {}
    public:
        static bool Register(const char* name, Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free);
        static bool Register(const char* name, mtf_u8* buf, mtf_u32 len);
        static MTF_IMemory_c* GetMemory(const char* name);
        static void* Malloc_s(mtf_i32 size);
        static void* Realloc(void* block, int32_t size);
        static void* Calloc(int32_t count, int32_t size);
        static void Free(void* block);
        static void* MallocName(const char* name, mtf_i32 size);
        static void* ReallocName(const char* name, void* block, int32_t size);
        static void* CallocName(const char* name, int32_t count, int32_t size);
        static void FreeName(const char* name, void* block);
    public:
        static bool DeRegister(const char* name, mtf_u8* buf, mtf_u32 len);
    public:
        Item* _items = 0;
        mtf_i32 _numMax = 0;
        Item* _gbl = 0;
    };
    //MTF_Memory_c* MemoryItems();
}

