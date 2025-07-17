#pragma once
// #include "plc_base.h"
#include <stdint.h>
#include "plc_base_port_c.h"

class plc_register_c
{
private:
    typedef void* (*create_t)(void *ptr);
    typedef uint32_t (*get_size_t)();
    typedef void (*destory_t)(void *ptr);
    class Item_c{
    public:
        Item_c(){}
        ~Item_c(){}
    public:
        char *_name = 0;
        create_t _create = 0;
        get_size_t _get_szie = 0;
        destory_t _destory = 0;
    };
public:
    plc_register_c(){}
    ~plc_register_c(){}
public:
    template<class T>
    static void Register(const char* name){
        register_(
          name
        , [](void* ptr) {return (void*)new(ptr) T(); }
        , []() {return (uint32_t)sizeof(T); }
        , [](void* ptr) {((T*)ptr)->~T(); });
    }
    static void* Create(const char* name, plc_base_port_c* plc_base_port);
    //static void Destory(const char* name, plc_base_port_c* plc_base_port, void *buf);
    static void Print(plc_base_port_c* plc_base_port);
private:
    static void register_(const char* name, create_t create, get_size_t get_szie, destory_t destory);
public:
    constexpr const static uint32_t _items_num = 6;
    static Item_c _items[_items_num];
    static plc_register_c _plc_register;
};

