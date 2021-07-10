#include "mem_pool.h"

#ifdef _NUSYS_
#include <nustd/malloc.h>

#define MEM_ALLOC(SIZE, MEMPOOL) ((MEMPOOL) ? mem_zone_alloc((MEMPOOL), (SIZE)) : malloc(SIZE))
#else
#define MEM_ALLOC(SIZE, MEMPOOL) (mem_zone_alloc((MEMPOOL), (SIZE)))
#endif