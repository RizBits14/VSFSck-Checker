#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "vsfs.h"

#define MAX_BLOCKS 64

int is_bit_set(uint8_t *bitmap, int index) {
    return (bitmap[index / 8] >> (index % 8)) & 1;
}

int main() {
    FILE *fp = fopen("vsfs.img", "rb");
    if (!fp) {
        perror("Error opening vsfs.img");
        return 1;
    }

    struct superblock sb;
    fread(&sb, sizeof(struct superblock), 1, fp);

    uint8_t data_bitmap[BLOCK_SIZE];
    fseek(fp, sb.data_bitmap_block * BLOCK_SIZE, SEEK_SET);
    fread(data_bitmap, 1, BLOCK_SIZE, fp);

    int inode_referenced[BLOCK_COUNT] = {0};

    for (int i = 0; i < sb.inode_count; i++) {
        struct inode temp_inode;
        fseek(fp, sb.inode_table_start * BLOCK_SIZE + i * sb.inode_size, SEEK_SET);
        fread(&temp_inode, sizeof(struct inode), 1, fp);

        if (temp_inode.links > 0 && temp_inode.deletion_time == 0) {
            int db = temp_inode.direct_block;

            if (db >= sb.first_data_block && db < BLOCK_COUNT) {
                inode_referenced[db] = 1;
            }
        }
    }

    printf("Checking Data Bitmap Consistency...\n");

    int errors = 0;
    for (int i = sb.first_data_block; i < BLOCK_COUNT; i++) {
        if (is_bit_set(data_bitmap, i) && !inode_referenced[i]) {
            printf("❌ Block %d is marked used in bitmap but not referenced by any inode.\n", i);
            errors++;
        }
    }

    for (int i = sb.first_data_block; i < BLOCK_COUNT; i++) {
        if (!is_bit_set(data_bitmap, i) && inode_referenced[i]) {
            printf("❌ Block %d is used by inode but not marked used in bitmap.\n", i);
            errors++;
        }
    }

    if (errors == 0) {
        printf("✅ All data bitmap entries are consistent.\n");
    }

    fclose(fp);
    return 0;
}

