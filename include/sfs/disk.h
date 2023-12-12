/**
 * @file disk.h
 * @author 计21-1 戴杰 20101050226
 * @brief 简单的磁盘模拟器，通过单个文件来模拟disk
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef DISK_H
#define DISK_H
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

#define BLOCK_SIZE 4096   //logic block size

// Disk structure
typedef struct Disk {
  FILE* disk_io;            // file pointer to the disk
  size_t blocks_num;        // number of blocks
  size_t writes_num;        // number of writes
  size_t reads_num;         // number of reads
  bool is_mounted;          // disk is mounted or not
} Disk;

// Disk operations
Disk *disk_open(const char *path, size_t blocks_num);
void disk_close(Disk* disk);

ssize_t disk_read(Disk *disk, size_t block, char *data);
ssize_t disk_write(Disk *disk, size_t block, char *data);

#endif
