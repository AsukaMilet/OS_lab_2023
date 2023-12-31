#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "argparse.h"
#include "memory/buffer_manager.h"
#include "memory/replacer.h"

const unsigned int INSTRUCTIONS_NUM = 320;

// Generate the index of instruction
size_t generate_index(size_t lower, size_t upper);

void lru_epoch(size_t frames_num, size_t replacer_k);

void fifo_epoch(size_t frames_num);

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: memory --random seed\n");
    exit(EXIT_FAILURE);
  }
  int seed = 0;
  // Parse the command line
  struct argparse_option options[] = {OPT_HELP(), OPT_INTEGER('s', "seed", &seed, "random seed", NULL, 0, 0),
                                      OPT_END()};
  struct argparse parse;
  argparse_init(&parse, options, NULL, 0);
  argc = argparse_parse(&parse, argc, argv);
  srand(seed);

  unsigned int *instructions = (unsigned int *)malloc(INSTRUCTIONS_NUM * sizeof(unsigned int));
  for (int i = 0; i < 320; i++) instructions[i] = i;

  for (int i = 4; i < 33; ++i) {
    printf("Current Memory Size: %d Frames\n", i);
    fifo_epoch(i);
    lru_epoch(i, 1);
    lru_epoch(i, 3);
    printf("\n\n");
  }
  free(instructions);
  return 0;
}

size_t generate_index(size_t lower, size_t upper) { return (rand() % (upper - lower + 1)) + lower; }

void lru_epoch(size_t frames_num, size_t replacer_k) {
  const size_t PAGE_SIZE = 10;
  LRUBufferManager *manager = LRUBufferManagerInit(frames_num, replacer_k);
  int access_num = 0;
  while ((unsigned int)access_num < INSTRUCTIONS_NUM) {
    // Random generate a start index
    size_t start = generate_index(0, INSTRUCTIONS_NUM);
    // Execute instruction at m+1
    page_id_t access_page = (start + 1) / PAGE_SIZE;
    frame_id_t frame = LRUBufferManagerFetchPage(manager, access_page);
    if (frame == -1) {
      fprintf(stderr, "Error: Something wrong in FetchPage\n");
    }
    access_num++;
    // Random pick a instruction between 0 and m+1
    size_t forward_jump = generate_index(0, start + 1);
    // Execute instruction at m'
    access_page = forward_jump / PAGE_SIZE;
    frame = LRUBufferManagerFetchPage(manager, access_page);
    if (frame == -1) {
      fprintf(stderr, "Error: Something wrong in FetchPage\n");
    }
    access_num++;
    // Execute instruction at m'+1
    access_page = (forward_jump + 1) / PAGE_SIZE;
    frame = LRUBufferManagerFetchPage(manager, access_page);
    if (frame == -1) {
      fprintf(stderr, "Error: Something wrong in FetchPage\n");
    }
    access_num++;
    // Random pick a instruction between m'+2 and 319
    size_t backward_jump = generate_index(forward_jump + 2, INSTRUCTIONS_NUM);
    access_page = backward_jump / PAGE_SIZE;
    frame = LRUBufferManagerFetchPage(manager, access_page);
    if (frame == -1) {
      fprintf(stderr, "Error: Something wrong in FetchPage\n");
    }
    access_num++;
  }

  size_t compulsory_miss_num = 0;
  size_t capacity_miss_num = 0;
  LRUBufferManagerGetMissNum(manager, &compulsory_miss_num, &capacity_miss_num);
  printf("LRU-%zu Missing Rate: %.2lf\n", replacer_k,
         (double)(compulsory_miss_num + capacity_miss_num) / INSTRUCTIONS_NUM);

  LRUBufferManagerDestroy(manager);
}

void fifo_epoch(size_t frames_num) {
  const size_t PAGE_SIZE = 10;
  FIFOBufferManager *manager = FIFOBufferManagerInit(frames_num);
  int access_num = 0;
  while ((unsigned int)access_num < INSTRUCTIONS_NUM) {
    // Random generate a start index
    size_t start = generate_index(0, INSTRUCTIONS_NUM);
    // Execute instruction at m+1
    page_id_t access_page = (start + 1) / PAGE_SIZE;
    frame_id_t frame = FIFOBufferManagerFetchPage(manager, access_page);
    if (frame == -1) {
      fprintf(stderr, "Error: Something wrong in FetchPage\n");
    }
    access_num++;
    // Random pick a instruction between 0 and m+1
    size_t forward_jump = generate_index(0, start + 1);
    // Execute instruction at m'
    access_page = forward_jump / PAGE_SIZE;
    frame = FIFOBufferManagerFetchPage(manager, access_page);
    if (frame == -1) {
      fprintf(stderr, "Error: Something wrong in FetchPage\n");
    }
    access_num++;
    // Execute instruction at m'+1
    access_page = (forward_jump + 1) / PAGE_SIZE;
    frame = FIFOBufferManagerFetchPage(manager, access_page);
    if (frame == -1) {
      fprintf(stderr, "Error: Something wrong in FetchPage\n");
    }
    access_num++;
    // Random pick a instruction between m'+2 and 319
    size_t backward_jump = generate_index(forward_jump + 2, INSTRUCTIONS_NUM);
    access_page = backward_jump / PAGE_SIZE;
    frame = FIFOBufferManagerFetchPage(manager, access_page);
    if (frame == -1) {
      fprintf(stderr, "Error: Something wrong in FetchPage\n");
    }
    access_num++;
  }

  size_t compulsory_miss_num = 0;
  size_t capacity_miss_num = 0;
  FIFOBufferManagerGetMissNum(manager, &compulsory_miss_num, &capacity_miss_num);
  printf("FIFO Missing Rate: %.2lf\n", (double)(compulsory_miss_num + capacity_miss_num) / INSTRUCTIONS_NUM);

  FIFOBufferManagerDestroy(manager);
}
