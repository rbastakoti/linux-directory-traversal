#ifndef INODE_H_INCLUDED
#define INODE_H_INCLUDED


struct Inode {
uint16_t i_mode;
uint16_t i_uid;
uint32_t i_size;
uint32_t i_atime;
uint32_t i_ctime;
uint32_t i_mtime;
uint32_t i_dtime;
uint16_t i_gid;
uint16_t i_links_count;
uint32_t i_blocks;
uint32_t i_flags;
uint32_t i_osd1;
uint32_t i_block[15];
uint32_t i_generation;
uint32_t i_file_acl;
uint32_t i_sizeHigh;
uint32_t i_faddr;
uint16_t i_blocksHigh;
uint16_t reserved16;
uint16_t i_uidHigh;
uint16_t i_gidHigh;
uint32_t reserved32;
}__attribute__((packed));

#endif // INODE_H_INCLUDED
