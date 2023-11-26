#include <stddef.h>
#include "memory/replacer.h"
#include "memory/buffer_manager.h"

//===----------------------------------------------------------------------===//
// LRUBufferManager Implementation
//===----------------------------------------------------------------------===//

LRUBufferManager *LRUBufferManagerInit(size_t pool_size, size_t replacer_k) {
  LRUBufferManager *manager = (LRUBufferManager *)malloc(sizeof(LRUBufferManager));
  manager->pool_size = pool_size;
  manager->capacity_miss_num_ = 0;
  manager->compulsory_miss_num_ = 0;
  manager->free_list_ = FreeList_init();
  manager->page_table_ = PageTable_init();
  manager->pages_ = (page_id_t *)malloc(sizeof(page_id_t) * pool_size);
  manager->replacer_ = ReplacerInit(pool_size, replacer_k);

  // Initially, every page is in the free list.
  for (int i = 0; i < (int)pool_size; ++i) {
    FreeList_push_back(&manager->free_list_, i);
  }

  return manager;
}

void LRUBufferManagerDestroy(LRUBufferManager *manager) {
  FreeList_drop(&manager->free_list_);
  PageTable_drop(&manager->page_table_);
  ReplacerDestroy(manager->replacer_);
  free(manager->pages_);
  free(manager);
}

frame_id_t LRUBufferManagerFetchPage(LRUBufferManager *manager, page_id_t page_id) {
  // Given page_id is in the page table
  if (PageTable_contains(&manager->page_table_, page_id)) {
    const frame_id_t frame_id = *PageTable_at(&manager->page_table_, page_id);
    ReplacerRecordAccess(manager->replacer_, frame_id);
    ReplacerSetEvictable(manager->replacer_, frame_id, true);
    return frame_id;
  }

  // Given page_id is not in the page table
  if (!FreeList_empty(&manager->free_list_)) {
    // Allocate a new frame from the free list front
    const frame_id_t frame_id = *FreeList_front(&manager->free_list_);
    FreeList_pop_front(&manager->free_list_);
    manager->pages_[frame_id] = page_id;
    PageTable_insert(&manager->page_table_, page_id, frame_id);
    ReplacerRecordAccess(manager->replacer_, frame_id);
    ReplacerSetEvictable(manager->replacer_, frame_id, true);
    manager->compulsory_miss_num_++;
    return frame_id;
  }

  // Free list is empty, should evict a existing frame
  frame_id_t frame_id;
  if (ReplacerEvict(manager->replacer_, &frame_id)) {
    const page_id_t old_page = manager->pages_[frame_id];
    PageTable_erase(&manager->page_table_, old_page);
    manager->pages_[frame_id] = page_id;
    PageTable_insert(&manager->page_table_, page_id, frame_id);
    ReplacerRecordAccess(manager->replacer_, frame_id);
    ReplacerSetEvictable(manager->replacer_, frame_id, true);
    manager->capacity_miss_num_++;
    return frame_id;
  }
  return -1;
}

void LRUBufferManagerGetMissNum(LRUBufferManager *manager, size_t *compulsory_miss_num, size_t *capacity_miss_num) {
  *compulsory_miss_num = manager->compulsory_miss_num_;
  *capacity_miss_num = manager->capacity_miss_num_;
}
//===----------------------------------------------------------------------===//
// FIFOBufferManager implementation
//===----------------------------------------------------------------------===//

FIFOBufferManager *FIFOBufferManagerInit(size_t pool_size) {
  FIFOBufferManager *manager = (FIFOBufferManager *)malloc(sizeof(FIFOBufferManager));
  manager->pool_size = pool_size;
  manager->capacity_miss_num_ = 0;
  manager->compulsory_miss_num_ = 0;
  manager->free_list_ = FreeList_init();
  manager->page_table_ = PageTable_init();
  manager->pages_ = (page_id_t *)malloc(sizeof(page_id_t) * pool_size);
  manager->replacer_ = FIFOReplacerInit(pool_size);

  // Initially, every page is in the free list.
  for (int i = 0; i < (int)pool_size; ++i) {
    FreeList_push_back(&manager->free_list_, i);
  }

  return manager;
}

void FIFOBufferManagerDestroy(FIFOBufferManager *manager) {
  FreeList_drop(&manager->free_list_);
  PageTable_drop(&manager->page_table_);
  FIFOReplacerDestroy(manager->replacer_);
  free(manager->pages_);
  free(manager);
}

frame_id_t FIFOBufferManagerFetchPage(FIFOBufferManager *manager, page_id_t page_id) {
  // Given page_id is in the page table
  if (PageTable_contains(&manager->page_table_, page_id)) {
    const frame_id_t frame_id = *PageTable_at(&manager->page_table_, page_id);
    FIFOReplacerRecordAccess(manager->replacer_, frame_id);
    FIFOReplacerSetEvictable(manager->replacer_, frame_id, true);
    return frame_id;
  }

  // Given page_id is not in the page table
  if (!FreeList_empty(&manager->free_list_)) {
    // Allocate a new frame from the free list front
    const frame_id_t frame_id = *FreeList_front(&manager->free_list_);
    FreeList_pop_front(&manager->free_list_);
    manager->pages_[frame_id] = page_id;
    PageTable_insert(&manager->page_table_, page_id, frame_id);
    FIFOReplacerRecordAccess(manager->replacer_, frame_id);
    FIFOReplacerSetEvictable(manager->replacer_, frame_id, true);
    manager->compulsory_miss_num_++;
    return frame_id;
  }

  // Free list is empty, should evict a existing frame
  frame_id_t frame_id;
  if (FIFOReplacerEvict(manager->replacer_, &frame_id)) {
    const page_id_t old_page = manager->pages_[frame_id];
    PageTable_erase(&manager->page_table_, old_page);
    manager->pages_[frame_id] = page_id;
    PageTable_insert(&manager->page_table_, page_id, frame_id);
    FIFOReplacerRecordAccess(manager->replacer_, frame_id);
    FIFOReplacerSetEvictable(manager->replacer_, frame_id, true);
    manager->capacity_miss_num_++;
    return frame_id;
  }
  return -1;
}

void FIFOBufferManagerGetMissNum(FIFOBufferManager *manager, size_t *compulsory_miss_num, size_t *capacity_miss_num) {
  *compulsory_miss_num = manager->compulsory_miss_num_;
  *capacity_miss_num = manager->capacity_miss_num_;
}
