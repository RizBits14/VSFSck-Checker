#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "vsfs.h"

#define BLOCK_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <vsfs.img>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    Superblock sb;
    fread(&sb, sizeof(Superblock), 1, fp);
    fclose(fp);

    printf("Validating Superblock...\n");

    printf("Magic number: %s\n", sb.magic == 0xD34D ? "OK" : "INVALID");
    printf("Block size: %s\n", sb.block_size == 4096 ? "OK" : "INVALID");
    printf("Total blocks: %s\n", sb.fs_size == 64 ? "OK" : "INVALID");
    printf("Inode bitmap block: %s\n", sb.inode_bitmap_block == 1 ? "OK" : "INVALID");
    printf("Data bitmap block: %s\n", sb.data_bitmap_block == 2 ? "OK" : "INVALID");
    printf("Inode table start: %s\n", sb.inode_table_start == 3 ? "OK" : "INVALID");
    printf("First data block: %s\n", sb.data_block_start == 8 ? "OK" : "INVALID");
    printf("Inode size: %s\n", sb.inode_size == 256 ? "OK" : "INVALID");

    return 0;
}

