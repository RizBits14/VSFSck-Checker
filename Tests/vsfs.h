#ifndef VSFS_H
#define VSFS_H
#define BLOCK_SIZE 4096
#define TOTAL_BLOCKS 64
#define BLOCK_COUNT TOTAL_BLOCKS
#define INODE_SIZE 256
#define INODE_TABLE_BLOCKS 5
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
#define INODE_COUNT (INODES_PER_BLOCK * INODE_TABLE_BLOCKS)

#define MAGIC 0xD34D

typedef struct superblock {
	unsigned short magic;          
	unsigned int block_size;      
	unsigned int block_count;     	
	unsigned int inode_bitmap_block;  
	unsigned int data_bitmap_block;  
	unsigned int inode_table_start;   
	unsigned int first_data_block;	
	unsigned int inode_size;      	
	unsigned int inode_count;     
	char reserved[4058];          	
} Superblock;

typedef struct inode {
	unsigned int mode;
	unsigned int uid;
	unsigned int gid;
	unsigned int file_size;
	unsigned int access_time;
	unsigned int creation_time;
	unsigned int modification_time;
	unsigned int deletion_time;
	unsigned int links;           	
	unsigned int blocks;
	unsigned int direct_block;
	unsigned int single_indirect;
	unsigned int double_indirect;
	unsigned int triple_indirect;
	char reserved[156];           
} Inode;

#endif

