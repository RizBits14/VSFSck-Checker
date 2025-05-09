#ifndef VSFS_H
#define VSFS_H

#include <stdint.h>

#define BLOCK_SIZE 4096
#define TOTAL_BLOCKS 64
#define INODE_BITMAP_BLOCK 1
#define DATA_BITMAP_BLOCK 2
#define INODE_TABLE_START_BLOCK 3
#define INODE_TABLE_BLOCKS 5
#define DATA_BLOCK_START 8

// Superblock structure (Size: 4096 bytes)
typedef struct {
    uint16_t magic;
    uint32_t block_size;
    uint32_t fs_size;
    uint32_t inode_bitmap_block;
    uint32_t data_bitmap_block;
    uint32_t inode_table_start;
    uint32_t data_block_start;
    uint32_t inode_size;
    uint32_t inode_count;
    uint8_t reserved[4058];
} Superblock;

#endif

