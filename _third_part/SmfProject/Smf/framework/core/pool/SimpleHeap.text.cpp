#include "SimpleHeap.h"
#include "SimpleHeapNode.h"
#include "string.h"

using namespace smf;

void testSimpleHeap() {
	//auto nsize = sizeof(SimpleHeap::Node);
	static uint32_t buff[1024 * 32];
	SimpleHeap heap(buff, sizeof(buff), 1024); heap.Print();
	auto ptr = heap.Alloc(1111); heap.Print();
	heap.Free(ptr); heap.Print();
	ptr = heap.Alloc(1911); heap.Print();
	heap.Free((char*)ptr + 18); heap.Print();
	auto ptr0 = heap.Alloc(124); heap.Print();
	auto ptr1 = heap.Alloc(3951); heap.Print();
	auto ptr2 = heap.Alloc(133); heap.Print();
	auto ptr3 = heap.Alloc(2131); heap.Print();
	auto ptr4 = heap.Alloc(199); heap.Print();
	auto ptr5 = heap.Alloc(3333); heap.Print();
	heap.Free(ptr3); heap.Print();
	heap.Free(ptr2); heap.Print();
	heap.Free(ptr4); heap.Print();
	heap.Free(ptr1); heap.Print();
	heap.Free(ptr0); heap.Print();
	heap.Free(ptr5); heap.Print();
	//дoverflow��ָ��ڵ�
	ptr = heap.Alloc(32); heap.Print();
	memset(ptr, 0, 64);
	heap.Free(ptr); heap.Print();
	//
}