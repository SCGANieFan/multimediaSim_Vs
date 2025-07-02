#include "plc_base.h"
#include "plc_register_c.h"

plc_register_c::Item_c plc_register_c::_items[plc_register_c::_items_num];
plc_register_c plc_register_c::_plc_register;

void* plc_register_c::Create(const char* name, plc_base_port_c* plc_base_port){
    for(uint32_t n=0;n<_items_num;n++){
        Item_c *item = &_plc_register._items[n];
        if (!item->_name) { continue; }
        if(!strcmp(item->_name,name)){
            void *buf = plc_base_port->malloc(item->_get_szie());
            if(!buf){return 0;}
            item->_create(buf);
            return buf;
        }
    }
    return 0;
}
#if 0
void plc_register_c::Destory(const char* name, plc_base_port_c* plc_base_port, void *buf){
    for(uint32_t n=0;n<_items_num;n++){
        Item_c *item = &_plc_register._items[n];
        if(!strcmp(item->_name,name)){
            item->_destory(buf);
            plc_base_port->free(buf);
            return;
        }
    }
}
#endif
void plc_register_c::Print(plc_base_port_c* plc_base_port){
    for(uint32_t n=0;n<_items_num;n++){
        Item_c *item = &_plc_register._items[n];
        if(item->_name){
            LOG(plc_base_port->print_cb, "support %s",item->_name);
        }
    }
}

void plc_register_c::register_(const char* name, create_t create, get_size_t get_szie, destory_t destory){
    for(uint32_t n=0;n<_items_num;n++){
        Item_c *item = &_plc_register._items[n];
        if(!item->_name
            ||!strcmp(item->_name,name)){
            item->_name = (char*)name;
            item->_create = create;
            item->_get_szie = get_szie;
            item->_destory = destory;
            return;
        }
    }
}
    
    
