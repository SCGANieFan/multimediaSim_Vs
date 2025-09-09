#if 1
#include "MTF.Memory.h"
#include "MTF.String.h"
#include "MTF.Printer.h"
using namespace mtf_ns;

//MTF_MemoryItems<2> _items;
//static MTF_MemoryItems<2> _memoryItems;

MTF_OMemoryItems_c<2> _memoryItems;
MTF_MemoryItems_c* MemoryItems() {
    return &_memoryItems;
}


void MTF_Memory_c::Init(Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free)
{
    _malloc = malloc;
    _realloc = realloc;
    _calloc = calloc;
    _free = free;
}

void MTF_Memory_c::DeInit()
{
    _malloc = 0;
    _realloc = 0;
    _calloc = 0;
    _free = 0;
}

void* MTF_Memory_c::Malloc(int32_t size)
{
    if (_malloc)
        return _malloc(size);
    return NULL;
}
void* MTF_Memory_c::Realloc(void* block, int32_t size)
{
    if (_realloc)
        return _realloc(block, size);
    return NULL;
}
void* MTF_Memory_c::Calloc(int32_t count, int32_t size)
{
    if (_calloc)
        return _calloc(count, size);
    return NULL;
}
void MTF_Memory_c::Free(void* block)
{
    if (_free)
        return _free(block);
}

void MTF_MemoryItems_c::Register(const char* name, Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free) {
    for (mtf_u16 n = 0; n < _numMax; n++) {
        Item *item = &_items[n];
        if (!item->_id) {
            item->_name = name;
            item->_id = *(mtf_u32*)name;
            item->_memory.Init(malloc, realloc, calloc, free);
            if (STR2ID64NL("gbl") == STR2ID64NL(name)) {
                if (_gbl) {
                    MTF_PRINT("warn, _gbl has eixt, %p -> %p", _gbl, item->_memory);
                }
                _gbl = &item->_memory;
            }
        }
    }
}
void MTF_MemoryItems_c::MTF_MemoryItems_c::Register(const char* name, mtf_u8* buf, mtf_u32 len) {
}
MTF_Memory_c* MTF_MemoryItems_c::GetMemory(const char* name) {
    mtf_u32 id = *(mtf_u32*)name;
    for (mtf_u16 n = 0; n < _numMax; n++) {
        Item* item = &_items[n];
        if (item->_id == id) {
            return &item->_memory;
        }
    }
}
namespace mtf_ns {
bool MTF_MemoryRegister(const char* name, Malloc_cb malloc, Realloc_cb realloc, Calloc_cb calloc, Free_cb free) {
    MTF_MemoryItems_c* items = MemoryItems();
    items->Register(name, malloc, realloc, calloc, free);
    return true;
}
bool MTF_MemoryRegister(const char* name, mtf_u8* buf, mtf_u32 len) {
    MTF_MemoryItems_c* items = MemoryItems();
    items->Register(name, buf, len);
    return true;
}

MTF_Memory_c* MTF_MemoryGetMemory(const char* name) {
    MTF_MemoryItems_c* items = MemoryItems();
    return items->GetMemory(name);
}

void* Malloc(mtf_i32 size) {
    MTF_MemoryItems_c* items = MemoryItems();
    if (items->_gbl)
        return items->_gbl->Malloc(size);
    return 0;
}
void* MallocName(const char* name, mtf_i32 size) {
    MTF_Memory_c* memory = MTF_MemoryGetMemory(name);
    if (memory)
        return memory->Malloc(size);
    return 0;
}

void* Realloc(void* block, int32_t size) {
    MTF_MemoryItems_c* items = MemoryItems();
    if (items->_gbl)
        return items->_gbl->Realloc(block, size);
    return 0;
}
void* ReallocName(const char* name, void* block, int32_t size) {
    MTF_Memory_c* memory = MTF_MemoryGetMemory(name);
    if (memory)
        return memory->Realloc(block, size);
    return 0;
}


void* Calloc(int32_t count, int32_t size) {
    MTF_MemoryItems_c* items = MemoryItems();
    if (items->_gbl)
        return items->_gbl->Calloc(count, size);
    return 0;
}
void* CallocName(const char* name, int32_t count, int32_t size) {
    MTF_Memory_c* memory = MTF_MemoryGetMemory(name);
    if (memory)
        return memory->Calloc(count, size);
    return 0;
}

void Free(void* block) {
    MTF_MemoryItems_c* items = MemoryItems();
    if (items->_gbl)
        items->_gbl->Free(block);
}
void FreeName(const char* name, void* block) {
    MTF_Memory_c* memory = MTF_MemoryGetMemory(name);
    if (memory)
        return memory->Free(block);
}
}

#endif