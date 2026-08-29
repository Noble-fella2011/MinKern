#ifndef FS_H
#define FS_H

#include <stdint.h>

#define FS_MAGIC 0x4D594F53
#define FS_BLOCK_SIZE 4096

struct superblock {
    uint32_t magic;
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t file_table_start;
    uint32_t file_table_blocks;
    uint32_t data_start;
};

struct file {
    char name[32];
    uint32_t size;
    uint32_t first_block;
    uint8_t type;
};

void fs_init(void);
struct file *fs_open(const char *name);
int fs_read(struct file *file, void *buffer, uint32_t size);
void fs_list(void);

#endif
