#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>

#define VSFS_MAGIC 0xD34D
#define BLOCK_SIZE 4096
#define TOTAL_BLOCKS 64
#define INODE_SIZE 256
#define INODE_COUNT 80

// Superblock structure
typedef struct {
    uint16_t magic;
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t inode_bitmap_block;
    uint32_t data_bitmap_block;
    uint32_t inode_table_start;
    uint32_t data_block_start;
    uint32_t inode_size;
    uint32_t inode_count;
    uint8_t reserved[4058];
} superblock_t;

// Inode structure
typedef struct {
    uint32_t mode;
    uint32_t uid, gid;
    uint32_t size;
    uint32_t atime, ctime, mtime, dtime;
    uint32_t links_count;
    uint32_t blocks_count;
    uint32_t direct_blocks[10];
    uint32_t single_indirect;
    uint32_t double_indirect;
    uint32_t triple_indirect;
    uint8_t reserved[156];
} inode_t;

superblock_t superblock;
uint8_t inode_bitmap[BLOCK_SIZE];
uint8_t data_bitmap[BLOCK_SIZE];
inode_t inodes[INODE_COUNT];
bool block_referenced[TOTAL_BLOCKS] = {0};
int block_reference_count[TOTAL_BLOCKS] = {0};
bool errors_found = false;

int is_bitmap_set(uint8_t *bitmap, int bit) {
    return bitmap[bit / 8] & (1 << (bit % 8));
}

void set_bitmap_bit(uint8_t *bitmap, int bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

void clear_bitmap_bit(uint8_t *bitmap, int bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

void read_superblock(int fd) {
    lseek(fd, 0, SEEK_SET);
    read(fd, &superblock, sizeof(superblock));
}

void read_bitmaps(int fd) {
    lseek(fd, superblock.inode_bitmap_block * BLOCK_SIZE, SEEK_SET);
    read(fd, inode_bitmap, BLOCK_SIZE);
    lseek(fd, superblock.data_bitmap_block * BLOCK_SIZE, SEEK_SET);
    read(fd, data_bitmap, BLOCK_SIZE);
}

void read_inodes(int fd) {
    for (int i = 0; i < superblock.inode_count; i++) {
        off_t offset = superblock.inode_table_start * BLOCK_SIZE + i * INODE_SIZE;
        lseek(fd, offset, SEEK_SET);
        read(fd, &inodes[i], sizeof(inode_t));
    }
}

void check_superblock() {
    if (superblock.magic != VSFS_MAGIC) {
        printf("ERROR: Invalid magic number. Fixing...\n");
        superblock.magic = VSFS_MAGIC;
        errors_found = true;
    }
    if (superblock.block_size != BLOCK_SIZE) {
        printf("ERROR: Invalid block size. Fixing...\n");
        superblock.block_size = BLOCK_SIZE;
        errors_found = true;
    }
    if (superblock.total_blocks != TOTAL_BLOCKS) {
        printf("ERROR: Invalid total blocks. Fixing...\n");
        superblock.total_blocks = TOTAL_BLOCKS;
        errors_found = true;
    }
    if (superblock.inode_size != INODE_SIZE) {
        printf("ERROR: Invalid inode size. Fixing...\n");
        superblock.inode_size = INODE_SIZE;
        errors_found = true;
    }
    if (superblock.inode_count != INODE_COUNT) {
        printf("ERROR: Invalid inode count. Fixing...\n");
        superblock.inode_count = INODE_COUNT;
        errors_found = true;
    }
}

void check_inode_bitmap() {
    for (int i = 0; i < INODE_COUNT; i++) {
        bool in_use = (inodes[i].links_count > 0 && inodes[i].dtime == 0);
        bool bitmap_set = is_bitmap_set(inode_bitmap, i);
        if (in_use && !bitmap_set) {
            set_bitmap_bit(inode_bitmap, i);
            errors_found = true;
        } else if (!in_use && bitmap_set) {
            clear_bitmap_bit(inode_bitmap, i);
            errors_found = true;
        }
    }
}

void check_data_blocks(int fd) {
    for (int i = 0; i < superblock.data_block_start; i++) {
        block_referenced[i] = true;
    }
    for (int i = 0; i < INODE_COUNT; i++) {
        if (inodes[i].links_count == 0 || inodes[i].dtime != 0) continue;
        for (int j = 0; j < 10; j++) {
            uint32_t blk = inodes[i].direct_blocks[j];
            if (blk >= superblock.data_block_start && blk < TOTAL_BLOCKS) {
                block_referenced[blk] = true;
                block_reference_count[blk]++;
            } else if (blk != 0) {
                inodes[i].direct_blocks[j] = 0;
                errors_found = true;
            }
        }
    }
    for (int i = superblock.data_block_start; i < TOTAL_BLOCKS; i++) {
        bool used = is_bitmap_set(data_bitmap, i);
        if (block_referenced[i] && !used) {
            set_bitmap_bit(data_bitmap, i);
            errors_found = true;
        } else if (!block_referenced[i] && used) {
            clear_bitmap_bit(data_bitmap, i);
            errors_found = true;
        }
    }
}

void write_superblock(int fd) {
    lseek(fd, 0, SEEK_SET);
    write(fd, &superblock, sizeof(superblock));
}

void write_bitmaps(int fd) {
    lseek(fd, superblock.inode_bitmap_block * BLOCK_SIZE, SEEK_SET);
    write(fd, inode_bitmap, BLOCK_SIZE);
    lseek(fd, superblock.data_bitmap_block * BLOCK_SIZE, SEEK_SET);
    write(fd, data_bitmap, BLOCK_SIZE);
}

void write_inodes(int fd) {
    for (int i = 0; i < superblock.inode_count; i++) {
        off_t offset = superblock.inode_table_start * BLOCK_SIZE + i * INODE_SIZE;
        lseek(fd, offset, SEEK_SET);
        write(fd, &inodes[i], sizeof(inode_t));
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filesystem_image>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("Failed to open filesystem image");
        return 1;
    }

    read_superblock(fd);
    read_bitmaps(fd);
    read_inodes(fd);

    check_superblock();
    check_inode_bitmap();
    check_data_blocks(fd);

    if (errors_found) {
        printf("Fixing filesystem errors...\n");
        write_superblock(fd);
        write_bitmaps(fd);
        write_inodes(fd);
        printf("Filesystem repaired.\n");
    } else {
        printf("No errors found. Filesystem is consistent.\n");
    }

    close(fd);
    return 0;
}

