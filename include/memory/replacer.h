#ifndef MEMORY_H
#define MEMORY_H

#define i_type UIList
#define i_key size_t
// Include type size_t
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "stc/clist.h"

typedef int32_t frame_id_t;

typedef struct LRUKNode {
  UIList history_;
  size_t k_;
  frame_id_t fid_;
  bool is_evictable_;
} LRUKNode;

//===----------------------------------------------------------------------===//
// LRUKNode statement
//===----------------------------------------------------------------------===//
void LRUKNodeInit(LRUKNode *node, size_t k, frame_id_t fid);

void LRUKNodeDestroy(LRUKNode *node);

bool IsEvictable(LRUKNode *node);

void SetEvictable(LRUKNode *node, bool set_evictable);

void FrameAccessed(LRUKNode *node, size_t timestamp);

size_t TimestampNum(LRUKNode *node);

size_t OldestTimestamp(LRUKNode *node);

frame_id_t GetFrameId(LRUKNode *node);

size_t KthTimestamp(LRUKNode *node);
//===----------------------------------------------------------------------===//
// Replacer statement
//===----------------------------------------------------------------------===//

#endif
