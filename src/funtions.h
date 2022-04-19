#ifndef FUNCTIONS
#define FUNCTIONS
#include "vdiHeader.h"
#include "vdi.h"
#include "partitionEntry.h"
#include "partition.h"
#include "superBlock.h"
#include "blockDescriptor.h"
#include "inode.h"
#include "DirectoryEntry.h"
#include "directory.h"

//VDI I/O functions
struct vdiFile *vdiOpen (char *fn);
void vdiClose (struct vdiFile *f);
ssize_t vdiRead(struct vdiFile *f, void *buf, size_t count);
ssize_t vdiWrite( struct vdiFile *f, void *buf, size_t count);
off_t vdiSeek(struct vdiFile *f, off_t offset, int anchor);

//Partition I/O function
struct PartitionFile *partitionOpen(struct vdiFile *vdiFile, struct PartitionEntry partitionEntry);
void partitionClose(struct PartitionFile *f);
ssize_t partitionRead(struct PartitionFile *partitionFile,void *buf,size_t count);
ssize_t partitionWrite(struct PartitionFile *f,void *buf,size_t count);
off_t partitionSeek(struct PartitionFile *f,off_t offset,int anchor);
void dumpPartitionEntry(struct PartitionEntry partitionEntry);
void partitonEntryTable( struct vdiFile* vdiFile,PartitionEntry *partitionEntry);

//Functions for Ext2file access
struct Ext2File *ext2Open (char *fn, int32_t pNum);
void ext2Close (struct Ext2File *f);
int32_t fetchBlock(struct Ext2File *f,uint32_t blockNum, void *buf);
int32_t writeBlock(struct Ext2File *f,uint32_t blockNum, void *buf);
int32_t fetchSuperblock(struct Ext2File *f,uint32_t blockNum, struct SuperBlock *sb);
int32_t writeSuperblock(struct Ext2File *f,uint32_t blockNum, struct SuperBlock *sb);
int32_t fetchBGDT(struct Ext2File *f,uint32_t blockNum,struct BlockDescriptor *blockDescriptor);
int32_t writeBGDT(struct Ext2File *f,uint32_t blockNum,struct BlockDescriptor *BlockDescriptor);
void dumpBlockGroupDescriptorTable (struct Ext2File *f);
void dumpBlockGroupDescriptorTableEntry (BlockDescriptor *blockDescriptor);

//Functions for I_node
int32_t fetchInode(struct Ext2File *f,uint32_t i_Num, struct Inode *buf);
int32_t writeInode(struct Ext2File *f,uint32_t i_Num, struct Inode *buf);
int32_t inodeInUse(struct Ext2File *f,uint32_t i_Num);
uint32_t allocateInode(struct Ext2File *f,int32_t group);
int32_t freeInode(struct Ext2File *f,uint32_t i_Num);
void dumpInode(Inode *inode);


//Functions for data blocks
int32_t fetchBlockFromFile(struct Ext2File *f,struct Inode *i,uint32_t bNum, void *buf);
int32_t writeBlockToFile(struct Ext2File *f,struct Inode *i,uint32_t bNum, void *buf,uint32_t iNum);
int32_t allocateBlock(struct Ext2File *f, uint32_t groupNumber);
int32_t blockInUse (struct Ext2File *f,uint32_t blockGroupNumber,uint32_t bNum);

//Functions to access directories
struct Directory *openDir(struct  Ext2File *f, uint32_t iNum);
bool getNextDirent(struct Directory *d,uint32_t &iNum,char *name);
void rewindDir(struct Directory *d);
void closeDir(struct Directory *d);

//to search a directory
uint32_t searchDir(struct Ext2File *f,uint32_t iNum,char *target);
//splitting a Path
uint32_t traversePath(Ext2File *f);
//Copy file from host to
void copyFunction(char *vdiFileName, char *hostFilePath, Ext2File*f);

void displayBuffer(char *buffer, int size);

void displayFiles(Ext2File *f, uint32_t dNum);
void displayFilesWithInfo(Ext2File *f, uint32_t dirNum);
#endif // FUNCTIONS
