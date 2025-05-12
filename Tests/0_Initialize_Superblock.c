#include <stdio.h>
#include <stdlib.h>
#include "vsfs.h"

int main() {
    FILE *fs = fopen("vsfs.img", "r+b"); 
    if (!fs) {
        perror("Error opening file system image");
        return 1;
    }

    Superblock sb;
    sb.magic = MAGIC;
    sb.block_size = BLOCK_SIZE;
    sb.block_count = BLOCK_COUNT;
    sb.inode_bitmap_block = 1;
    sb.data_bitmap_block = 2;
    sb.inode_table_start = 3;
    sb.first_data_block = 4;
    sb.inode_size = INODE_SIZE;
    sb.inode_count = INODE_COUNT;

    if (fwrite(&sb, sizeof(Superblock), 1, fs) != 1) {
        perror("Failed to write superblock");
        fclose(fs);
        return 1;
    }

    fclose(fs);
    printf("Superblock written successfully to vsfs.img\n");
    return 0;
}

