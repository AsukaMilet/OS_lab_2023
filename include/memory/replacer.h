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

typedef struct Frame {
  UIList history_;
  size_t k_;
  frame_id_t fid_;
  bool is_evictable_;
} Frame;

//===----------------------------------------------------------------------===//
// Frame statement
//===----------------------------------------------------------------------===//
Frame FrameInit(size_t k, frame_id_t fid);

Frame Frame_clone(Frame node);

// Destructor for Frame
void FrameDestroy(Frame *node);

// Check whether a frame is evictable
bool IsEvictable(Frame *node);

// Set whether a frame is evictable
void SetEvictable(Frame *node, bool set_evictable);

// Record the access for the given frame
void FrameAccessed(Frame *node, size_t timestamp);

// Get the number of access for the given frame
size_t TimestampNum(Frame *node);

// Get the earliest access for the given frame
size_t OldestTimestamp(Frame *node);

frame_id_t GetFrameId(Frame *node);

// Get the kth access for the given frame
size_t KthTimestamp(Frame *node);
//===----------------------------------------------------------------------===//
// Replacer statement
//===----------------------------------------------------------------------===//
#define i_type FrameTable
#define i_key frame_id_t
#define i_valclass Frame
#define i_valclone Frame_clone
#define i_valdrop FrameDestroy
#include "stc/cmap.h"

typedef struct LRUKReplacer {
  FrameTable node_store_;
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
void ReplacerRecordAccess(Replacer *replacer, frame_id_t frame_id);

// Toggle whether a frame is evictable or non-evictable
void ReplacerSetEvictable(Replacer *replacer, frame_id_t frame_id, bool set_evictable);

// Return replacer's size, which tracks the number of evictable frames
size_t ReplacerSize(Replacer *replacer);
//===----------------------------------------------------------------------===//
// FIFO Replacer statement
//===----------------------------------------------------------------------===//
typedef struct FIFOReplacer {
  FrameTable node_store_;
  size_t current_timestamp_;
  size_t curr_size_;      // The number of evictable frames
  size_t replacer_size_;  // Maximum number of frames in the replacer
} FIFOReplacer;

// Initialize the replacer
FIFOReplacer *FIFOReplacerInit(size_t num_frames);

// Destroy the replacer to avoid memory leak
void FIFOReplacerDestroy(FIFOReplacer *replacer);

// Evict the frame with earliest timestamp
bool FIFOReplacerEvict(FIFOReplacer *replacer, frame_id_t *frame_id);

// Record the access of a frame
void FIFOReplacerRecordAccess(FIFOReplacer *replacer, frame_id_t frame_id);

// Toggle whether a frame is evictable or non-evictable
void FIFOReplacerSetEvictable(FIFOReplacer *replacer, frame_id_t frame_id, bool set_evictable);

// Return replacer's size, which tracks the number of evictable frames
size_t FIFOReplacerSize(FIFOReplacer *replacer);
#endif
