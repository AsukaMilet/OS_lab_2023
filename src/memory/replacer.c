#include "memory/replacer.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

//===----------------------------------------------------------------------===//
// Frame Implementation
//===----------------------------------------------------------------------===//
Frame FrameInit(size_t k, frame_id_t fid) {
  Frame node;
  node.history_ = UIList_init();
  node.k_ = k;
  node.fid_ = fid;
  node.is_evictable_ = false;
  return node;
}

void FrameDestroy(Frame *node) { UIList_drop(&node->history_); }

// Check whether a frame is evictable
bool IsEvictable(Frame *node) { return node->is_evictable_; }

// Set whether a frame is evictable
void SetEvictable(Frame *node, bool set_evictable) { node->is_evictable_ = set_evictable; }

void FrameAccessed(Frame *node, size_t timestamp) {
  // New timestamp is added to the front of the timestamp list
  UIList_push_front(&node->history_, timestamp);
}

size_t TimestampNum(Frame *node) {
  // Return the number of timestamps
  return UIList_count(&node->history_);
}

size_t OldestTimestamp(Frame *node) {
  // Return the oldest timestamp
  return *UIList_back(&node->history_);
}

frame_id_t GetFrameId(Frame *node) { return node->fid_; }

// Get the kth timestamp
size_t KthTimestamp(Frame *node) {
  UIList_iter iter = UIList_begin(&node->history_);
  iter = UIList_advance(iter, node->k_ - 1);
  return *iter.ref;
}

Frame Frame_clone(Frame node) {
  node.history_ = UIList_clone(node.history_);
  node.fid_ = node.fid_;
  node.k_ = node.k_;
  node.is_evictable_ = node.is_evictable_;
  return node;
}
//===----------------------------------------------------------------------===//
// Replacer Implementation
//===----------------------------------------------------------------------===//
Replacer *ReplacerInit(size_t num_frames, size_t k) {
  Replacer *replacer = (Replacer *)malloc(sizeof(Replacer));
  replacer->node_store_ = FrameTable_init();
  replacer->current_timestamp_ = 0;
  replacer->curr_size_ = 0;
  replacer->replacer_size_ = num_frames;
  replacer->k_ = k;
  return replacer;
}

void ReplacerDestroy(Replacer *replacer) {
  FrameTable_drop(&replacer->node_store_);
  free(replacer);
}

bool ReplacerEvict(Replacer *replacer, frame_id_t *frame_id) {
  // If no frame is evictable, return false directly
  if (replacer->curr_size_ == 0) {
    return false;
  }

  frame_id_t evict_frame_id;
  size_t k_distance = 0;
  size_t oldest_recent_timestamp = UINT_MAX;

  c_forpair(frame_id, frame, FrameTable, replacer->node_store_) {
    // Check whether the frame is evictable
    if (IsEvictable(_.frame)) {
      // If frame access is less than k-distance, k-distance is +inf
      if (TimestampNum(_.frame) < replacer->k_) {
        k_distance = UINT_MAX;
        if (OldestTimestamp(_.frame) < oldest_recent_timestamp) {
          // If the k-distance are equal, choose the earliest frame had been accessed
          oldest_recent_timestamp = OldestTimestamp(_.frame);
          evict_frame_id = *_.frame_id;
        }
      } else {
        // Calculate the k-distance, find the maximum
        if (replacer->current_timestamp_ - KthTimestamp(_.frame) > k_distance) {
          k_distance = replacer->current_timestamp_ - KthTimestamp(_.frame);
          evict_frame_id = *_.frame_id;
        } else if (replacer->current_timestamp_ - KthTimestamp(_.frame) == k_distance) {
          // If the k-distance are equal, choose the earliest frame had been accessed
          if (OldestTimestamp(_.frame) < oldest_recent_timestamp) {
            oldest_recent_timestamp = OldestTimestamp(_.frame);
            evict_frame_id = *_.frame_id;
          }
        }
      }
    }
  }

  *frame_id = evict_frame_id;
  FrameTable_erase(&replacer->node_store_, evict_frame_id);
  replacer->curr_size_--;
  return true;
}

void ReplacerRecordAccess(Replacer *replacer, frame_id_t frame_id) {
  // Frame id is invalid
  if ((size_t)frame_id > replacer->replacer_size_) {
    fprintf(stderr, "frame_id should be less than replacer_size\n");
  }

  // If the frame has not been seen before, insert it
  Frame temp = FrameInit(replacer->k_, frame_id);
  FrameTable_insert(&replacer->node_store_, frame_id, temp);
  Frame *frame_ptr = FrameTable_at_mut(&replacer->node_store_, frame_id);
  FrameAccessed(frame_ptr, replacer->current_timestamp_);
  replacer->current_timestamp_++;
}

void ReplacerSetEvictable(Replacer *replacer, frame_id_t frame_id, bool set_evictable) {
  // If the frame doesn't exist, directly return
  if (!FrameTable_contains(&replacer->node_store_, frame_id)) {
    fprintf(stderr, "Frame %d doesn't exist\n", frame_id);
    return;
  }

  Frame *frame_ptr = FrameTable_at_mut(&replacer->node_store_, frame_id);

  bool original_evictable = IsEvictable(frame_ptr);
  // If the evictable field of the given frame has not changed, return directly
  if (set_evictable == original_evictable) {
    return;
  }

  // A frame was previously non-evictable and is to be set to evictable
  if (!original_evictable) {
    SetEvictable(frame_ptr, set_evictable);
    replacer->curr_size_++;
  } else {
    // A frame was previously evictable and is to be set to non-evictable
    SetEvictable(frame_ptr, set_evictable);
    replacer->curr_size_--;
  }
}

size_t ReplacerSize(Replacer *replacer) { return replacer->curr_size_; }
//===----------------------------------------------------------------------===//
// FIFO Replacer implementation
//===----------------------------------------------------------------------===//
FIFOReplacer *FIFOReplacerInit(size_t num_frames) {
  FIFOReplacer *replacer = (FIFOReplacer *)malloc(sizeof(FIFOReplacer));
  replacer->node_store_ = FrameTable_init();
  replacer->current_timestamp_ = 0;
  replacer->curr_size_ = 0;
  replacer->replacer_size_ = num_frames;
  return replacer;
}

void FIFOReplacerDestroy(FIFOReplacer *replacer) {
  FrameTable_drop(&replacer->node_store_);
  free(replacer);
}

bool FIFOReplacerEvict(FIFOReplacer *replacer, frame_id_t *frame_id) {
  // If no frame is evictable, return false directly
  if (replacer->curr_size_ == 0) {
    return false;
  }

  frame_id_t evict_frame_id;
  size_t oldest_recent_timestamp = UINT_MAX;

  c_forpair(frame_id, frame, FrameTable, replacer->node_store_) {
    // Check whether the frame is evictable
    if (IsEvictable(_.frame)) {
      // Choose the earliest frame had been accessed
      if (OldestTimestamp(_.frame) < oldest_recent_timestamp) {
        oldest_recent_timestamp = OldestTimestamp(_.frame);
        evict_frame_id = *_.frame_id;
      }
    }
  }
  
  *frame_id = evict_frame_id;
  FrameTable_erase(&replacer->node_store_, evict_frame_id);
  replacer->curr_size_--;
  return true;
}

void FIFOReplacerRecordAccess(FIFOReplacer *replacer, frame_id_t frame_id) {
  // Frame id is invalid
  if ((size_t)frame_id > replacer->replacer_size_) {
    fprintf(stderr, "frame_id should be less than replacer_size\n");
  }

  // If the frame has not been seen before, insert it
  Frame temp = FrameInit(0, frame_id);
  FrameTable_insert(&replacer->node_store_, frame_id, temp);
  Frame *frame_ptr = FrameTable_at_mut(&replacer->node_store_, frame_id);
  FrameAccessed(frame_ptr, replacer->current_timestamp_);
  replacer->current_timestamp_++;
}

void FIFOReplacerSetEvictable(FIFOReplacer *replacer, frame_id_t frame_id, bool set_evictable) {
  // If the frame doesn't exist, directly return
  if (!FrameTable_contains(&replacer->node_store_, frame_id)) {
    fprintf(stderr, "Frame %d doesn't exist\n", frame_id);
    return;
  }

  Frame *frame_ptr = FrameTable_at_mut(&replacer->node_store_, frame_id);

  bool original_evictable = IsEvictable(frame_ptr);
  // If the evictable field of the given frame has not changed, return directly
  if (set_evictable == original_evictable) {
    return;
  }

  // A frame was previously non-evictable and is to be set to evictable
  if (!original_evictable) {
    SetEvictable(frame_ptr, set_evictable);
    replacer->curr_size_++;
  } else {
    // A frame was previously evictable and is to be set to non-evictable
    SetEvictable(frame_ptr, set_evictable);
    replacer->curr_size_--;
  }
}

size_t FIFOReplacerSize(FIFOReplacer *replacer) { return replacer->curr_size_; }
