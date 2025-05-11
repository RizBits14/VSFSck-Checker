#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "vsfs.h"

#define INODE_TABLE_BLOCK 3
#define INODE_SIZE 256

int main() {
    FILE *fp = fopen("vsfs.img", "rb");
    if (!fp) {
        perror("Failed to open vsfs.img");
        return 1;
    }

    struct superblock sb;
    fread(&sb, sizeof(struct superblock), 1, fp);

    // Read inode bitmap block
    uint8_t inode_bitmap[BLOCK_SIZE];
    fseek(fp, sb.inode_bitmap_block * BLOCK_SIZE, SEEK_SET);
    fread(inode_bitmap, BLOCK_SIZE, 1, fp);

    // Read all inodes and check consistency
    fseek(fp, sb.inode_table_start * BLOCK_SIZE, SEEK_SET);

    int errors_found = 0;
    for (int i = 0; i < sb.inode_count; i++) {
        struct inode temp_inode;
        fseek(fp, sb.inode_table_start * BLOCK_SIZE + (i * INODE_SIZE), SEEK_SET);
        fread(&temp_inode, sizeof(struct inode), 1, fp);

        int bitmap_used = (inode_bitmap[i / 8] >> (i % 8)) & 1;
        
        // Check inode validity using 'links' field
        int inode_valid = (temp_inode.links > 0) && (temp_inode.deletion_time == 0);

        if (bitmap_used && !inode_valid) {
            printf("Inode %d is marked used in bitmap but is invalid.\n", i);
            errors_found++;
        }
        if (!bitmap_used && inode_valid) {
            printf("Inode %d is valid but not marked used in bitmap.\n", i);
            errors_found++;
        }
    }

    if (errors_found == 0) {
        printf("Inode bitmap is consistent.\n");
    }

    fclose(fp);
    return 0;
}

