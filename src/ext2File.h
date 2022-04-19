#ifndef EXT2FILE_H_INCLUDED
#define EXT2FILE_H_INCLUDED
#include "partition.h"
#include "superBlock.h"
#include "blockDescriptor.h"

struct Ext2File{
    PartitionFile *partitionFile;
    SuperBlock superblock;
    size_t numberofBlockGroups;
    BlockDescriptor* blockDescriptor;
}__attribute__((packed));

#endif // EXT2FILE_H_INCLUDED
