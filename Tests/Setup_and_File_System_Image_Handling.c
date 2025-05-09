#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 4096
#define TOTAL_BLOCKS 64

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

    printf("File system image '%s' opened successfully.\n", argv[1]);

    fclose(fs);
    return 0;
}

// === Dummy image file of 64 blocks === //
// dd if=/dev/zero of=vsfs.img bs=4096 count=64 //
