#include "sfs/disk.h"

#include <stdio.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

/**
 * @brief Opens disk at specified path with the specified number of blocks
 *
 * @param[in] path  Path to disk file
 * @param[in] blocks_num Number of blocks to allocate for disk image
 * @return Disk*
 */
Disk *disk_open(const char *path, size_t blocks_num) {
  Disk *disk = (Disk *)malloc(sizeof(Disk));
  // Allocate error
  if (disk == NULL) {
    return NULL;
  }
  disk->disk_io = fopen(path, "ab+");
  // If directory or file does not exist, create it
  if(disk->disk_io == NULL){
    disk->disk_io = fopen(path, "wb+");
  }
  if (disk->disk_io == NULL) {
    free(disk);
    return NULL;
  }
  disk->blocks_num = blocks_num;
  disk->writes_num = 0;
  disk->reads_num = 0;
  disk->is_mounted = false;
  return disk;
}

/**
 * @brief Close the disk file and print number of writes and reads
 *
 * @param disk
 */
void disk_close(Disk *disk) {
  printf("Number of writes: %zu. Number of reads: %zu\n", disk->writes_num, disk->reads_num);
  fclose(disk->disk_io);
  free(disk);
}

/**
 * @brief Check whether the operation is valid
 *
 * @param disk
 * @param block
 * @param data
 * @return true
 * @return false
 */
bool disk_operation_valid_check(Disk *disk, size_t block, const char *data) {
  if (disk == NULL || data == NULL) {
    return false;
  }
  if (block >= disk->blocks_num) {
    return false;
  }
  return true;
}

/**
 * @brief Write data to disk at specified block from data buffer
 *
 * @param disk Pointer to the disk structure
 * @param block Block Number to perform write
 * @param data
 * @return ssize_t Number of bytes written(BLOCK_SIZE for success)
 */
ssize_t disk_write(Disk *disk, size_t block, char *data) {
  ssize_t bytes_written;
  if (!disk_operation_valid_check(disk, block, data)) {
    return -1;
  }
  off_t offset = block * BLOCK_SIZE;
  if (fseek(disk->disk_io, offset, SEEK_SET) == -1) {
    return -1;  // Error seeking to block
  }

  bytes_written = fwrite(data, sizeof(char), BLOCK_SIZE, disk->disk_io);
  // Flush buffer to disk
  fflush(disk->disk_io);
  if (bytes_written < BLOCK_SIZE) {
    return -1;
  }
  disk->writes_num += 1;
  return bytes_written;
}

/**
 * @brief Read data from disk at specified block into data buffer
 *
 * @param disk
 * @param block
 * @param data
 * @return size_t
 */
ssize_t disk_read(Disk *disk, size_t block, char *data) {
  ssize_t bytes_read;
  if (!disk_operation_valid_check(disk, block, data)) {
    return -1;
  }
  off_t offset = block * BLOCK_SIZE;
  if (fseek(disk->disk_io, offset, SEEK_SET) == -1) {
    return -1;  // Error seeking to block
  }
  
  bytes_read = fread(data, sizeof(char), BLOCK_SIZE, disk->disk_io);
  if (bytes_read < BLOCK_SIZE) {
    return -1;  // Error reading from disk
  }
  disk->reads_num += 1;
  return bytes_read;
}
