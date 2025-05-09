#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "vsfs.h"

#define MAX_BLOCKS 64

int main() {
    FILE *fp = fopen("vsfs.img", "rb");
    if (!fp) {
        perror("Failed to open vsfs.img");
        return 1;
    }

    struct superblock sb;
    fread(&sb, sizeof(struct superblock), 1, fp);

    int block_refs[MAX_BLOCKS] = {0};
    int errors_found = 0;

    for (int i = 0; i < sb.inode_count; i++) {
        struct inode temp_inode;
        fseek(fp, sb.inode_table_block * BLOCK_SIZE + (i * sb.inode_size), SEEK_SET);
        fread(&temp_inode, sizeof(struct inode), 1, fp);

        if (temp_inode.hard_links <= 0 || temp_inode.deletion_time != 0)
            continue;

        // Check direct block
        uint32_t b = temp_inode.direct_block;
        if (b >= sb.total_blocks || b < sb.first_data_block) {
            printf("Inode %d has invalid direct block reference: %u\n", i, b);
            errors_found++;
        } else {
            block_refs[b]++;
        }

        // You can extend this to indirect, double, and triple blocks if needed
    }

    // Detect duplicates
    for (int i = sb.first_data_block; i < sb.total_blocks; i++) {
        if (block_refs[i] > 1) {
            printf("Data block %d is referenced by multiple inodes (%d times).\n", i, block_refs[i]);
            errors_found++;
        }
    }

    if (errors_found == 0) {
        printf("No duplicate or bad block references found.\n");
    }

    fclose(fp);
    return 0;
}

