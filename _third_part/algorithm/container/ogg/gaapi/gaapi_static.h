#pragma once
#include "gaapi_base.h"
#include "gaapi.h"
#include "gaapi_gaf.h"
#include "gaapi_os.h"
namespace ogg_gaapi_ns {

    class GaapiGafRegister_c :public GaapiBase_c
    {
    public:
        class Item_c {
        public:
            uint32_t _id = 0;
            FuncCreate_t _funcCreate = 0;
        };
    public:
        GaapiGafRegister_c(Item_c* items, uint32_t itemNum);
        ~GaapiGafRegister_c();
    public:
        void Register(const char* type, FuncCreate_t funcCreate);
        GaapiGaf_c* Create(const char* type, GaapiBasePort_t* bp);
    public:
        uint32_t _itemNum;
        Item_c* _items;
        GaapiMutex_c _mtx;
    };

    template<uint32_t Num>
    class GaapiGafRegisterInstance_c :public GaapiGafRegister_c {
    public:
        GaapiGafRegisterInstance_c() : GaapiGafRegister_c(_items, Num) {}
        ~GaapiGafRegisterInstance_c() {}
    public:
        Item_c _items[Num];
    };

    class GaapiIdManager_c :public GaapiBase_c {
    public:
        class Item_c {
        public:
            Gaapi_c* _api = 0;
            uint32_t _id = 0;
        };
    public:
        GaapiIdManager_c(Item_c* items, uint32_t itemNum);
        ~GaapiIdManager_c();
    public:
        uint32_t Add(Gaapi_c* api);
        uint32_t Api2Id(Gaapi_c* api);
        Gaapi_c* Id2Api(uint32_t id);
        Gaapi_c* Remove(Gaapi_c* api);
        uint32_t Remove(uint32_t id);
    public:
        uint32_t _itemNum;
        Item_c* _items;
        GaapiMutex_c _mtx;
    };

    template<uint32_t Num>
    class GaapiIdManagerInstance_c :public GaapiIdManager_c {
    public:
        GaapiIdManagerInstance_c() :GaapiIdManager_c(items, Num) {}
        ~GaapiIdManagerInstance_c() {}
    public:
        Item_c items[Num];
    };

    class GaapiStatic_c :public GaapiBase_c {
    public:
        GaapiStatic_c();
        ~GaapiStatic_c();
    public:
        uint64_t _magic0;
        uint64_t _magic1;

        GaapiIdManagerInstance_c<32> _infos;
        GaapiGafRegisterInstance_c<64> _gafRoot;

        uint64_t _magic2;
        uint64_t _magic3;
    };
    void GaapiInit();
    void GaapiDeinit();
    GaapiStatic_c* GaapiStatic();
    GaapiIdManager_c* GaapiIdManager();
    GaapiGafRegister_c* GaapiGafRegister();
};

