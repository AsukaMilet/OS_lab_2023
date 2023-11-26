#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H
#include <stddef.h>
#include "replacer.h"

typedef int32_t page_id_t;

#define i_type FreeList
#define i_key frame_id_t
#include "stc/clist.h"

#define i_type PageTable
#define i_key page_id_t
#define i_val frame_id_t
#include "stc/cmap.h"

//===----------------------------------------------------------------------===//
// LRUBufferManager statement
//===----------------------------------------------------------------------===//
typedef struct LRUBufferManager {
  size_t pool_size;
  size_t capacity_miss_num_;    // The number of evictions
  size_t compulsory_miss_num_;  // The number of misses that are compulsory
  FreeList free_list_;
  PageTable page_table_;
  Replacer *replacer_;
  page_id_t *pages_;
} LRUBufferManager;

LRUBufferManager *LRUBufferManagerInit(size_t pool_size, size_t replacer_k);

void LRUBufferManagerDestroy(LRUBufferManager *manager);

// Fetch a page from the buffer manager
frame_id_t LRUBufferManagerFetchPage(LRUBufferManager *manager, page_id_t pid);

void LRUBufferManagerGetMissNum(LRUBufferManager *manager, size_t *compulsory_miss_num, size_t *capacity_miss_num);
//===----------------------------------------------------------------------===//
// FIFOBufferManager statement
//===----------------------------------------------------------------------===//
typedef struct FIFOBufferManager {
  size_t pool_size;
  size_t capacity_miss_num_;    // The number of evictions
  size_t compulsory_miss_num_;  // The number of misses that are compulsory
  FreeList free_list_;
  PageTable page_table_;
  FIFOReplacer *replacer_;
  page_id_t *pages_;
} FIFOBufferManager;

FIFOBufferManager *FIFOBufferManagerInit(size_t pool_size);

void FIFOBufferManagerDestroy(FIFOBufferManager *manager);

// Fetch a page from the buffer manager
frame_id_t FIFOBufferManagerFetchPage(FIFOBufferManager *manager, page_id_t pid);

void FIFOBufferManagerGetMissNum(FIFOBufferManager *manager, size_t *compulsory_miss_num, size_t *capacity_miss_num);
#endif
