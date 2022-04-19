#ifndef BLOCKDESCRIPTOR_H_INCLUDED
#define BLOCKDESCRIPTOR_H_INCLUDED

struct BlockDescriptor{
    uint32_t 	bg_block_bitmap;
    uint32_t 	bg_inode_bitmap;
    uint32_t 	bg_inode_table;
    uint16_t 	bg_free_blocks_count;
    uint16_t 	bg_free_inodes_count;
    uint16_t 	bg_used_dirs_count;
    uint16_t 	bg_pad;
    uint32_t 	bg_reserved[3];
}__attribute__((packed));

#endif // BLOCKDESCRIPTOR_H_INCLUDED
