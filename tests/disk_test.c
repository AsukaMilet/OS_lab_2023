#include "sfs/disk.h"
#include "sfs/logging.h"

#include <assert.h>
#include <stdio.h>
#include <limits.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Constants */

#define DISK_BLOCKS (4)
const char DISK_PATH[] = "disk_test.image";

/* Functions */

void test_cleanup(void) {
  unlink(DISK_PATH);
}

int test_00_disk_open(void) {
  debug("Check bad path");
  Disk *disk = disk_open("/root/NOPE", 10);
  assert(disk == NULL);

  debug("Check disk attributes");
  disk = disk_open(DISK_PATH, 10);
  assert(disk);
  assert(disk->disk_io     != NULL);
  assert(disk->blocks_num  == 10);
  assert(disk->reads_num   == 0);
  assert(disk->writes_num  == 0);
  assert(disk->is_mounted == false);
  disk_close(disk);

  return EXIT_SUCCESS;
}

int test_01_disk_read(void) {
  Disk *disk = disk_open(DISK_PATH, DISK_BLOCKS);
  assert(disk);

  char* data = malloc(DISK_BLOCKS*BLOCK_SIZE);
  memset(data, '\0', DISK_BLOCKS*BLOCK_SIZE);
  char buf[BLOCK_SIZE] = {0};
  for(size_t i = 0; i < DISK_BLOCKS*BLOCK_SIZE; i++) {
    data[i] = i / BLOCK_SIZE;
  }
  assert(fwrite(data, sizeof(char), DISK_BLOCKS*BLOCK_SIZE, disk->disk_io) == DISK_BLOCKS*BLOCK_SIZE);
  fflush(disk->disk_io);
    
  debug("Check bad disk");
  assert(disk_read(NULL, 0, data) == -1);
    
  debug("Check bad block");
  assert(disk_read(disk, DISK_BLOCKS, data) == -1);
    
  debug("Check bad data");
  assert(disk_read(disk, DISK_BLOCKS, NULL) == -1);

  for (size_t b = 0; b < DISK_BLOCKS; b++) {
    debug("Check read block %lu", b);
    assert(disk_read(disk, b, buf) == BLOCK_SIZE);
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
      assert(buf[i] == b);
    }

    assert(disk->reads_num == b + 1);
  }
  free(data);
  disk_close(disk);
  return EXIT_SUCCESS;
}

int test_02_disk_write() {
  Disk *disk = disk_open(DISK_PATH, DISK_BLOCKS);
  assert(disk);
    
  char data[BLOCK_SIZE] = {0};

  debug("Check bad disk");
  assert(disk_write(NULL, 0, data) == -1);
    
  debug("Check bad block");
  assert(disk_write(disk, DISK_BLOCKS, data) == -1);
    
  debug("Check bad data");
  assert(disk_write(disk, DISK_BLOCKS, NULL) == -1);

  for (size_t b = 0; b < DISK_BLOCKS; b++) {
    debug("Check write block %lu", b);
    memset(data, b, BLOCK_SIZE);
    assert(disk_write(disk, b, data) == BLOCK_SIZE);

    memset(data, 0, BLOCK_SIZE);
    assert(disk_read(disk, b, data) == BLOCK_SIZE);

    for (size_t i = 0; i < BLOCK_SIZE; i++) {
      assert(data[i] == b);
    }

    assert(disk->writes_num == b + 1);
  }
  disk_close(disk);
  return EXIT_SUCCESS;
}

/* Main execution */

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s NUMBER\n\n", argv[0]);
    fprintf(stderr, "Where NUMBER is right of the following:\n");
    fprintf(stderr, "    0. Test disk_open\n");
    fprintf(stderr, "    1. Test disk_read\n");
    fprintf(stderr, "    2. Test disk_write\n");
    return EXIT_FAILURE;
  }

  int number = atoi(argv[1]);
  int status = EXIT_FAILURE;

  atexit(test_cleanup);

  switch (number) {
    case 0:  status = test_00_disk_open(); break;
    case 1:  status = test_01_disk_read(); break;
    case 2:  status = test_02_disk_write(); break;
    default: fprintf(stderr, "Unknown NUMBER: %d\n", number); break;
  }

  return status;
}