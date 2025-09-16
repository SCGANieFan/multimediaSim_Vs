#if 1
#include "MTF.Memory.h"
#include "MTF.String.h"
#include "MTF.Printer.h"

#include "MTF.MemoryBuff.h"
#include "MTF.MemoryCallback.h"

using namespace mtf_ns;

template<mtf_i32 NumMax>
class MTF_OMemoryItems_c :public MTF_Memory_c {
public:
    MTF_OMemoryItems_c() :MTF_Memory_c(_oitems, NumMax) {}
    ~MTF_OMemoryItems_c() {}
public:
    Item _oitems[NumMax];
};

MTF_OMemoryItems_c<2> _memoryItems;
MTF_Memory_c* MemoryItems() {
    return &_memoryItems;
}

namespace mtf_ns {
    bool MTF_Memory_c::Register(const char* name, Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free) {
        MTF_Memory_c* items = MemoryItems();
        for (mtf_u16 n = 0; n < items->_numMax; n++) {
            Item* item = &items->_items[n];
            if (!item->_id) {
                item->_name = name;
                item->_id = STR2ID32L(name);
                item->_type = STR2ID32L("callback");
                MTF_MemoryCallback_c* ptr = (MTF_MemoryCallback_c*)malloc(sizeof(MTF_MemoryCallback_c));
                new(ptr) MTF_MemoryCallback_c();
                ptr->Init(malloc, realloc, calloc, free);
                item->_memory = ptr;
                if (STR2ID64NL("gbl") == STR2ID64NL(name)) {
                    if (items->_gbl) {
                        MTF_PRINT("warn, _gbl has eixt, %p -> %p", items->_gbl, item->_memory);
                        if (items->_gbl->_type == STR2ID32L("callback")) {
                            items->_gbl->_memory->Free(items->_gbl->_memory);
                        }
                    }
                    items->_gbl = item;
                }
            }
        }
        return true;
    }

    bool MTF_Memory_c::MTF_Memory_c::Register(const char* name, mtf_u8* buf, mtf_u32 len) {
#if 1
        MTF_Memory_c* items = MemoryItems();
        for (mtf_u16 n = 0; n < items->_numMax; n++) {
            Item* item = &items->_items[n];
            if (!item->_id) {
                item->_name = name;
                item->_id = STR2ID32L(name);
                item->_type = STR2ID32L("buf");
                MTF_MemoryBuff_c* ptr = (MTF_MemoryBuff_c*)buf;
                new(ptr) MTF_MemoryBuff_c();
                ptr->Initialize(ptr + 1, len - sizeof(MTF_MemoryBuff_c));
                item->_memory = ptr;
                if (STR2ID64NL("gbl") == STR2ID64NL(name)) {
                    if (items->_gbl) {
                        MTF_PRINT("warn, _gbl has eixt, %p -> %p", items->_gbl, item->_memory);
                        if (items->_gbl->_type == STR2ID32L("callback")) {
                            items->_gbl->_memory->Free(items->_gbl->_memory);
                        }
                    }
                    items->_gbl = item;
                }
            }
        }
#endif
        return true;
    }

    MTF_IMemory_c* MTF_Memory_c::GetMemory(const char* name) {
        MTF_Memory_c* items = MemoryItems();
        mtf_u32 id = *(mtf_u32*)name;
        for (mtf_u16 n = 0; n < items->_numMax; n++) {
            Item* item = &items->_items[n];
            if (item->_id == id) {
                return item->_memory;
            }
        }
    }

    MTF_OMemoryItems_c<2> _memoryItems;
    namespace mtf_ns {
        MTF_Memory_c* MemoryItems() {
            return &_memoryItems;
        }
    }



    void* MTF_Memory_c::Malloc_s(mtf_i32 size) {
        MTF_Memory_c* items = MemoryItems();
        if (items->_gbl)
            return items->_gbl->_memory->Malloc(size);
        return 0;
    }
    void* MTF_Memory_c::Realloc(void* block, int32_t size) {
        MTF_Memory_c* items = MemoryItems();
        if (items->_gbl)
            return items->_gbl->_memory->Realloc(block, size);
        return 0;
    }
    void* MTF_Memory_c::Calloc(int32_t count, int32_t size) {
        MTF_Memory_c* items = MemoryItems();
        if (items->_gbl)
            return items->_gbl->_memory->Calloc(count, size);
        return 0;
    }
    void MTF_Memory_c::Free(void* block) {
        MTF_Memory_c* items = MemoryItems();
        if (items->_gbl)
            items->_gbl->_memory->Free(block);
    }
    void* MTF_Memory_c::MallocName(const char* name, mtf_i32 size) {
        MTF_IMemory_c* memory = GetMemory(name);
        if (memory)
            return memory->Malloc(size);
        return 0;
    }
    void* MTF_Memory_c::ReallocName(const char* name, void* block, int32_t size) {
        MTF_IMemory_c* memory = GetMemory(name);
        if (memory)
            return memory->Realloc(block, size);
        return 0;
    }
    void* MTF_Memory_c::CallocName(const char* name, int32_t count, int32_t size) {
        MTF_IMemory_c* memory = GetMemory(name);
        if (memory)
            return memory->Calloc(count, size);
        return 0;
    }
    void MTF_Memory_c::FreeName(const char* name, void* block) {
        MTF_IMemory_c* memory = GetMemory(name);
        if (memory)
            return memory->Free(block);
    }


}

#endif