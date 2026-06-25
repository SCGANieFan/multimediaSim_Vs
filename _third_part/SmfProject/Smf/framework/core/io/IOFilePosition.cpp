#include "IOFilePosition.h"
#include "SmfFCC.h"

using namespace smf;

EXTERNC void smf_io_file_pos_register() {
	IOFilePosition::Register<IOFilePosition>("io-file-pos");
}

EXTERNC void smf_io_buff_pos_register() {
	IOFilePosition::Register<IOFilePosition>("io-buff-pos");
}

IOFilePosition::IOFilePosition() {
    memset(_positions, 0, sizeof(_positions));
}

bool IOFilePosition::Open(void* para) {
    if(_io) {
        returnIfErrC(false, !_io->Open(para));
        returnIfErrC(false, !_io->Seek(_positions[0].keys, Position::front));
        return true;
    }
    unique_ptr<IO> io(IO::Create<IO>(_class->IDs(0), _class->IDs(1)));
    //unique_ptr<IO> io(IO::Create<IO>("io-file"));
    returnIfErrC(false, !io);
    returnIfErrC(false, !io->Open(para));
    auto para0 = (OpenParam*)para;
    if(para0->other) {
        dbgTestPPL(para0->other);
        initPositonList((smf_keys_value_t*)para0->other);
    }
    else if (para0->script) {
        dbgTestPSL(para0->script);
        initPositonList(para0->script);
    }
    _io = std::move(io);
    return true;
}

bool IOFilePosition::Close() {
    return _io ? _io->Close() : false;
}

int IOFilePosition::findPositonIdx(int& offset) {
    int size = 0;
    for(int i=0; i<32; ++i) {
        auto sz = _positions[i].vals;
        if(!sz) return -1;
        auto size0 = size + sz;
        if(size0 > offset) {
            offset = offset - size;
            return i;
        }
        size = size0;
    }
    return -1;
}

bool IOFilePosition::Seek(int offset, Position pos) {
    returnIfErrC(false, !_io);
    switch(pos) {
    case Position::current: offset += _offset; break;
    case Position::end: offset = _total + offset; break;
    default: break;
    }
    int suboft = offset;
    auto idx = findPositonIdx(suboft);
    returnIfErrC(false, idx < 0);
    returnIfErrC(false, !_io->Seek(_positions[idx].keys + suboft, Position::front));
    _curIdx = idx;
    _curOffset = suboft;
    _offset = offset;
    return true;
}

unsigned IOFilePosition::GetSize() const {
    if(!_total) {
        int total = 0;
        for(auto& pos : _positions) {
            if(!pos.vals) break;
            total += pos.vals;
        }
        _total = total;
    }
    return _total;
}

unsigned IOFilePosition::GetOffset() const  {
    return _offset;
}

unsigned IOFilePosition::Read(void* buff, unsigned size)  {
    returnIfErrC(false, !_io);
    auto idx = _curIdx;
    auto oft = _curOffset;
    auto data = (char*)buff;
    unsigned res = 0;
    auto io = (IO*)_io.get();
    for(; idx < 32; ++idx) {
        auto& item = _positions[idx];
        if(!item.vals) break;
        auto suboft = item.vals - oft;
        auto sz = suboft < size ? suboft : size;
        if(!io->Seek(item.keys + oft, Position::front)) {
            dbgWarnPXL("seek:%u", item.keys);
            break;
        }
        auto rsize = io->Read(data, sz);
        oft += rsize;
        size -= rsize;
        res += rsize;
        data += rsize;
        if(!size || (sz != rsize)) break;
        oft = 0;
    }
    _curIdx = idx;
    _curOffset = oft;
    _offset += res;
    return res;
}

unsigned IOFilePosition::Write(void* buff, unsigned size)  {
    return _io ? _io->Write(buff, size) : 0;
}

bool IOFilePosition::initPositonList(smf_keys_value_t* inlist) {
    returnIfWarnC(false, !inlist);
    uint32_t idx = 0;
    while(idx < 32 && inlist && inlist->vals) {
        _positions[idx++] = *inlist++;
    }
    return true;
}

bool IOFilePosition::initPositonList(const char* inlist) {
    //script:0-256,168-123
    returnIfWarnC(false, !inlist);
    auto ptr = inlist;
    uint32_t idx = 0;
    while (ptr && *ptr && idx < 32) {
        char* p = 0;
        auto offset = strtoul(ptr, &p, 0);
        if (*p != '-') {
            dbgErrPL();
            break;
        }
        ptr = p + 1;
        auto size = strtoul(ptr, &p, 0);
        ptr = p;
        _positions[idx++] = { offset, size };
    }
    _positions[idx++] = { 0, 0 };
    return true;
}

bool IOFilePosition::set(uint32_t key, void* val) {
    switch(key) {
    case Hash("posList"): {
        return initPositonList((smf_keys_value_t*)val);
    }
    }
    return IO::set(key, val);
}
