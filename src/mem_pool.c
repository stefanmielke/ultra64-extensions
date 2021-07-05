#include "../include/mem_pool.h"

#include <nustd/malloc.h>
#include <nustd/string.h>

void mem_zone_init(MemZone *z, char *mem_pool, int size) {
	void *ptr = mem_pool;
	z->pos = (char *)ptr;
	z->start = z->pos;
	z->end = z->start + size;
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
		return NULL;  // Out of memory. Put your error handling here.
	}

	void *ptr = (void *)z->pos;
	z->pos += size;
	return ptr;
}

void mem_zone_free_all(MemZone *z) {
	z->pos = z->start;
}