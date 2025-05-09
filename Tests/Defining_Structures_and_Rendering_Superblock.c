#include <stdio.h>
#include <stdlib.h>
#include "vsfs.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <vsfs.img>\n", argv[0]);
        return 1;
    }

    FILE *fs = fopen(argv[1], "rb");
    if (!fs) {
        perror("Error opening file system image");
        return 1;
    }

    Superblock sb;

    // Read the first block (Superblock)
    if (fread(&sb, sizeof(Superblock), 1, fs) != 1) {
        fprintf(stderr, "Failed to read superblock.\n");
        fclose(fs);
        return 1;
    }

    // Print superblock details
    printf("Magic Number: 0x%X\n", sb.magic);
    printf("Block Size: %u\n", sb.block_size);
    printf("Total Blocks: %u\n", sb.total_blocks);
    printf("Inode Bitmap Block: %u\n", sb.inode_bitmap_block);
    printf("Data Bitmap Block: %u\n", sb.data_bitmap_block);
    printf("Inode Table Start: %u\n", sb.inode_table_start);
    printf("First Data Block: %u\n", sb.first_data_block);
    printf("Inode Size: %u\n", sb.inode_size);
    printf("Inode Count: %u\n", sb.inode_count);

    fclose(fs);
    return 0;
}

