#ifndef MEMORY_H
#define MEMORY_H

#define i_type UIList
#define i_key size_t
// Include type size_t
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
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
LRUKNode LRUKNodeInit(size_t k, frame_id_t fid);

LRUKNode LRUKNode_clone(LRUKNode node);

// Destructor for LRUKNode
void LRUKNodeDestroy(LRUKNode *node);

// Check whether a frame is evictable
bool IsEvictable(LRUKNode *node);

// Set whether a frame is evictable
void SetEvictable(LRUKNode *node, bool set_evictable);

// Record the access for the given frame
void FrameAccessed(LRUKNode *node, size_t timestamp);

// Get the number of access for the given frame
size_t TimestampNum(LRUKNode *node);

// Get the earliest access for the given frame
size_t OldestTimestamp(LRUKNode *node);

frame_id_t GetFrameId(LRUKNode *node);

// Get the kth access for the given frame
size_t KthTimestamp(LRUKNode *node);
//===----------------------------------------------------------------------===//
// Replacer statement
//===----------------------------------------------------------------------===//
#define i_type HashTable
#define i_key frame_id_t
#define i_valclass LRUKNode
#define i_valclone LRUKNode_clone
#define i_valdrop LRUKNodeDestroy
#include "stc/cmap.h"

typedef struct LRUKReplacer {
  HashTable node_store_;
  size_t current_timestamp_;
  size_t curr_size_;      // The number of evictable frames
  size_t replacer_size_;  // Maximum number of frames in the replacer
  size_t k_;
} Replacer;

// Initialize the replacer
Replacer *ReplacerInit(size_t num_frames, size_t k);

// Destroy the replacer to avoid memory leak
void ReplacerDestroy(Replacer *replacer);

/**
 * @brief Find the frame with largest backward k-distance and evict that frame.
 *
 * @param replacer
 * @param[out] fid id of frame that is evicted.
 * @return true
 * @return false
 */
bool ReplacerEvict(Replacer *replacer, frame_id_t *fid);

// Record the access of a frame
void ReplacerRecordAccess(Replacer *replacer, frame_id_t fid);

// Toggle whether a frame is evictable or non-evictable
void ReplacerSetEvictable(Replacer *replacer, frame_id_t fid, bool set_evictable);

// Return replacer's size, which tracks the number of evictable frames
size_t ReplacerSize(Replacer *replacer);
#endif
