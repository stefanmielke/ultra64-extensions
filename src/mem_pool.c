#include "../include/mem_pool.h"

#include <ultra64.h>

#define abort() (*((char *)(NULL)) = 0)

void mem_zone_init(MemZone *z, char *mem_pool, int size) {
	z->start = mem_pool;
	z->end = z->start + size;
	z->pos = z->start;
}

void *mem_zone_alloc(MemZone *z, int size) {
	if (size == 0) {
		return NULL;
	}
	// Round up to multiple of 16 bytes.
	size = (size + 15) & ~(int)15;
	// How much free space remaining in zone?
	int rem = z->end - z->pos;
	if (rem < size) {
		abort();  // Out of memory. Causing a crash
		return NULL;
	}

	void *ptr = (void *)z->pos;
	z->pos += size;
	return ptr;
}

void mem_zone_free_all(MemZone *z) {
	z->pos = z->start;
}