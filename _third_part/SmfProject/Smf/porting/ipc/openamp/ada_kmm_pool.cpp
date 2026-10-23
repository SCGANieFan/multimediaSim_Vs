
#include <open-amp/rptun.h>

static inline void *kmm_malloc(size_t size) {
	return metal_allocate_memory(size);
}

static inline void *kmm_zalloc(size_t size) {
	auto *p = kmm_malloc(size);
	if (p == nullptr) {
		return p;
	}
	memset(p, 0, size);
	return p;
}

static inline void kmm_free(void *p) {
	metal_free_memory(p);
}
