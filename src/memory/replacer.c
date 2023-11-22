#include "memory/replacer.h"

//===----------------------------------------------------------------------===//
// LRUKNode Implementation
//===----------------------------------------------------------------------===//
void LRUKNodeInit(LRUKNode *node, size_t k, frame_id_t fid) {
  node->history_ = UIList_init();
  node->k_ = k;
  node->fid_ = fid;
  node->is_evictable_ = false;
}

void LRUKNodeDestroy(LRUKNode *node) {
  UIList_drop(&node->history_);
}

bool IsEvictable(LRUKNode *node) {
  return node->is_evictable_;
}

void SetEvictable(LRUKNode *node, bool set_evictable) {
  node->is_evictable_ = set_evictable;
}

void FrameAccessed(LRUKNode *node, size_t timestamp) {
  // New timestamp is added to the front of the timestamp list
  UIList_push_front(&node->history_, timestamp);
}

size_t TimestampNum(LRUKNode *node) {
  // Return the number of timestamps
  return UIList_count(&node->history_);
}

size_t OldestTimestamp(LRUKNode *node) {
  // Return the oldest timestamp
  return *UIList_back(&node->history_);
}

frame_id_t GetFrameId(LRUKNode *node) {
  return node->fid_;
}

// Get the kth timestamp
size_t KthTimestamp(LRUKNode *node) {
  UIList_iter iter = UIList_begin(&node->history_);
  iter = UIList_advance(iter, node->k_ - 1);
  return *iter.ref;
}
//===----------------------------------------------------------------------===//
// Replacer Implementation
//===----------------------------------------------------------------------===//
