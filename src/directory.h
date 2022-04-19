#ifndef DIRECTORY_H_INCLUDED
#define DIRECTORY_H_INCLUDED
#include "DirectoryEntry.h"
#include "ext2File.h"

struct Directory {
ssize_t cursor;
uint32_t iNum;
Inode inode;
uint8_t *ptr;
Ext2File *ext2File;
DirectoryEntry *directoryEntry;
};

#endif // DIRECTORY_H_INCLUDED
