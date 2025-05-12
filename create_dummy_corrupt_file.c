#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BLOCK_SIZE 1024
#define TOTAL_BLOCKS 100
#define INODE_COUNT 8
#define INODE_SIZE 64
#define VSFS_MAGIC 0x12345678

typedef struct {
    uint32_t magic;
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t inode_count;
    uint32_t inode_size;
    uint32_t inode_bitmap_block;
    uint32_t data_bitmap_block;
    uint32_t inode_table_start;
    uint32_t data_block_start;
} superblock_t;

typedef struct {
    uint16_t mode;
    uint16_t links_count;
    uint32_t size;
    uint32_t direct_blocks[10];
    uint32_t dtime;
} inode_t;

int main() {
    FILE *fp = fopen("corrupt_vsfs.img", "wb");
    if (!fp) {
        perror("File create failed");
        return 1;
    }

    superblock_t sb = {
        .magic = 0xdeadbeef,              
        .block_size = 512,                
        .total_blocks = 90,               
        .inode_count = INODE_COUNT,
        .inode_size = INODE_SIZE,
        .inode_bitmap_block = 1,
        .data_bitmap_block = 2,
        .inode_table_start = 3,
        .data_block_start = 5
    };
    fwrite(&sb, sizeof(sb), 1, fp);
    fseek(fp, BLOCK_SIZE - sizeof(sb), SEEK_CUR); 

    uint8_t inode_bitmap[BLOCK_SIZE] = {0xFF}; 
    fwrite(inode_bitmap, BLOCK_SIZE, 1, fp);
 
    uint8_t data_bitmap[BLOCK_SIZE] = {0};
    data_bitmap[0] = 0b11110000; 
    fwrite(data_bitmap, BLOCK_SIZE, 1, fp);

    inode_t inodes[INODE_COUNT];
    memset(inodes, 0, sizeof(inodes));
    
    inodes[0].links_count = 1;
    inodes[0].dtime = 0;
    inodes[0].direct_blocks[0] = 7;
    inodes[0].direct_blocks[1] = 7;

    inodes[1].links_count = 1;
    inodes[1].dtime = 0;
    inodes[1].direct_blocks[0] = 200;
    
    for (int i = 0; i < INODE_COUNT; i++) {
        fwrite(&inodes[i], sizeof(inode_t), 1, fp);
    }

    fseek(fp, TOTAL_BLOCKS * BLOCK_SIZE, SEEK_SET);
    fputc(0, fp);

    fclose(fp);
    printf("Created corrupt_vsfs.img with intentional errors.\n");
    return 0;
}

