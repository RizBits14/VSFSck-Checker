# vsfsck-checker
A simple file system checker for the Very Simple File System (VSFS), written in C. This tool reads a virtual file system image (vsfs.img), checks for errors in the `superblock, inodes, bitmaps, data blocks for duplicates and bad block.` Finally, reports or fixes the problems.

**Recommended**

The `Tests` folder has all the code dump in it. It has all the code divided into 6 individual parts

A **screenshot** here is attached for the execution of the .c files in the `Tests` folder

![Image](https://github.com/user-attachments/assets/090bab28-ad31-4b82-b59b-554756e6152e)

A corrupt file `corrupt_vsfs.img` was created intentionally by compiling the `create_dummy_corrupt_file.c` and executing the execution file `create_dummy_corrupt` to test the project. On the first execution the `corrupt_vsfs.img` file will get fixed and executing it again will show that the file system is consistent

![Image](https://github.com/user-attachments/assets/81260874-e718-4e1c-bb2e-6af53b048712)
