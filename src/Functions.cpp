#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <iomanip>
#include <string>
#include <cstddef>
#include <cmath>
#include <libgen.h>
#include <string.h>
#include "vdiHeader.h"
#include "vdi.h"

#include "partitionEntry.h"
#include "partition.h"
#include "superBlock.h"
#include "blockDescriptor.h"
#include "ext2File.h"
#include "inode.h"
#include "DirectoryEntry.h"
#include "directory.h"
#include "funtions.h"



using namespace std;

string filepath;

struct vdiFile *vdiOpen (char *f){
    //setting file descriptor to returned value from open as mentioned
    int fileDescripter = open(f,O_RDWR);


    vdiheader *header = {}; //purpose
    vdiFile *vdiFileStruct = new vdiFile;

    read(fileDescripter,&(vdiFileStruct->header),sizeof(vdiheader));
        //setting the file descriptor to value obtained from ad assigning cursor to 0
    vdiFileStruct->fileDescripter = fileDescripter; vdiFileStruct->cursor = 0;

    if (fileDescripter==-1){
        cout<< "Unable to Open" << endl;
        return NULL;
    }

    return vdiFileStruct;


}

void vdiClose(struct vdiFile* f){
close(f->fileDescripter);
}

//read the vdi file
ssize_t vdiRead (struct vdiFile *f,void *buf,size_t count){
    int readable;
    //move the cursor to the beginning of data
    lseek(f->fileDescripter, f->cursor + f->header.offData, SEEK_SET);
    //read from vdifile's disk space and store it into the buffer
    readable=read(f->fileDescripter,buf,count);
    if (readable ==-1){
        cout<< "Cant read the given bytes of data"<<endl;
        return NULL;

    }
    return readable;
}



off_t vdiSeek (vdiFile *f, off_t offset, int anchor){

    off_t position;
    position = lseek(f->fileDescripter, offset,anchor);
    if (position != -1){

    if (anchor == SEEK_SET){
        position = offset;
    }

    else if (anchor == SEEK_CUR){
        position = f->cursor+offset;
    }

    else if (anchor == SEEK_END){
        position = offset + f->header.diskSize;
    }
    f->cursor = position;
    return f->cursor;
    }

    return -1;

}



//Step2 -- PartitionFile functions

struct PartitionFile *partitionOpen(struct vdiFile *vdiFile, struct PartitionEntry partitionEntry){
PartitionFile *p = new PartitionFile;
//combining vdi and partition structure and returning pointer p
p->f = vdiFile;
p->partitionEntry = partitionEntry;

return p;
};

void partitionClose(struct PartitionFile* f){

}

ssize_t partitionRead( struct PartitionFile *partitionFile, void *buf, size_t count){
     if ( (partitionFile->f->cursor + count) >  (512 * (partitionFile->partitionEntry.blocking + partitionFile->partitionEntry.numberOfSector)) ){

        count = (partitionFile->partitionEntry.blocking+ partitionFile->partitionEntry.numberOfSector)*512- partitionFile->f->cursor;
     }
     return vdiRead(partitionFile->f,buf,count);

}

off_t partitionSeek(struct PartitionFile *f,off_t offset,int anchor){
    off_t location;

    int64_t cur = f->f->cursor;

    int64_t newCur;
    newCur = vdiSeek(f->f,offset+f->partitionEntry.blocking*512,anchor);


    if (newCur < f->partitionEntry.blocking*512 ||
        newCur >= 512 *(f->partitionEntry.blocking+f->partitionEntry.numberOfSector) )
        f->f->cursor = cur;


    return f->f->cursor - f->partitionEntry.blocking*512;
}

void partitionEntryTable( struct vdiFile* vdiFile,PartitionEntry *partitionEntry){

    //set the cursor to offet 446
    vdiSeek(vdiFile,446,SEEK_SET);
    //then read 64 bytes into the table
    vdiRead(vdiFile,partitionEntry,64);
    }



struct Ext2File *ext2Open(char *fn, int32_t pNum) {

Ext2File *ptr = new Ext2File;
vdiFile *vdiFile = vdiOpen(fn);

PartitionEntry *partitionEntry = NULL;
partitionEntry = new PartitionEntry[4];

partitionEntryTable(vdiFile, partitionEntry);

ptr->partitionFile = partitionOpen(vdiFile, partitionEntry[pNum]);

partitionSeek(ptr->partitionFile,1024, SEEK_SET);
partitionRead(ptr->partitionFile, &ptr->superblock,1024);



ptr->superblock.s_log_block_size = 1024 << ptr->superblock.s_log_block_size;

//assigning number of blockgroups using total blocks/ blocks per group
ptr->numberofBlockGroups = ptr->superblock.s_blocks_count/ptr->superblock.s_blocks_per_group;

ptr->blockDescriptor = new BlockDescriptor[ptr->numberofBlockGroups+1];


for (int i = 0; i <= ptr->numberofBlockGroups; i++){
    partitionSeek(ptr->partitionFile,(ptr->superblock.s_first_data_block+1) * ptr->superblock.s_log_block_size+ 32*i, SEEK_SET);
    //filling blockdescriptor table
    partitionRead(ptr->partitionFile, &ptr->blockDescriptor[i],32);
    }

 return ptr;
}


void ext2Close(struct Ext2File *f){
delete(f);
}

int32_t fetchBlock(struct Ext2File *f, uint32_t blockNum, void *buf) {

partitionSeek(f->partitionFile, blockNum*f->superblock.s_log_block_size, SEEK_SET);
//read
int32_t success = partitionRead(f->partitionFile, buf, f->superblock.s_log_block_size);

if(success == 0){
    return 0;
}else{
return 1;
}

}

int32_t fetchSuperblock(struct Ext2File *f,uint32_t blockNum, struct SuperBlock *sb){

    int groupNumber = round(blockNum/ f->superblock.s_blocks_per_group);
    //incase the block group is 0, we need to skip a block before accessing super-block
    if (groupNumber==0){

        partitionSeek(f->partitionFile,1024,SEEK_SET);
        //read 1024 data into the sb buffer.
        partitionRead(f->partitionFile,sb,1024);

        sb->s_log_block_size = 1024 << sb->s_log_block_size;
    }
    else{
        void *buff = malloc(f->superblock.s_log_block_size);
        int blockNumber = groupNumber * f->superblock.s_blocks_per_group +1;
        //reading super-block into the buffer
        fetchBlock(f,blockNumber,buff);
        memcpy(sb,buff,1024);

        //reseting the block size
        sb->s_log_block_size = 1024 << sb->s_log_block_size;
    }

    return 0;
}

int32_t fetchBGDT(struct Ext2File *f,uint32_t blockNum, BlockDescriptor *bgdt) {
    int groupNumber = round(blockNum/ f->superblock.s_blocks_per_group);
    //get inode bitmap
    bgdt->bg_inode_bitmap= f->blockDescriptor[groupNumber].bg_inode_bitmap;
    //get block bitmap
    bgdt->bg_block_bitmap=f->blockDescriptor[groupNumber].bg_block_bitmap;
    //get free block counts
    bgdt->bg_free_blocks_count=f->blockDescriptor[groupNumber].bg_free_blocks_count;
    //get free inode counts
    bgdt->bg_free_inodes_count=f->blockDescriptor[groupNumber].bg_free_inodes_count;
    //get used directory count
    bgdt->bg_used_dirs_count=f->blockDescriptor[groupNumber].bg_used_dirs_count;
    //get pad
    bgdt->bg_pad=f->blockDescriptor[groupNumber].bg_pad;
    //get inode table
    bgdt->bg_inode_table=f->blockDescriptor[groupNumber].bg_inode_table;


    for (int i = 0; i <3 ; ++i) {
        bgdt->bg_reserved[i]= f->blockDescriptor[groupNumber].bg_reserved[i];
    }

    return 0;
}


int32_t fetchInode(struct Ext2File *f,uint32_t iNum, struct Inode *buf){
    //subtract 1 from inum to deal with 1based array
    //find block group number
    int blockGroupNumber = (iNum-1)/f->superblock.s_inodes_per_group;
    iNum = (iNum - 1) % f->superblock.s_inodes_per_group;
    //find the number of inodes per block
    int perBlock= f->superblock.s_log_block_size/f->superblock.s_inode_size;

    int  inode_block_num = (iNum / perBlock) + f->blockDescriptor[blockGroupNumber].bg_inode_table;
    int offset_of_inode_in_block= iNum % perBlock;
    uint8_t * tempBlock = NULL;
    tempBlock= new uint8_t [f->superblock.s_log_block_size];
    fetchBlock(f,inode_block_num,tempBlock);
    *buf = ( (Inode *) tempBlock)[offset_of_inode_in_block];
    return 0;
}


int32_t fetchBlockFromFile(struct Ext2File *f,struct Inode *i,uint32_t bNum, void *buf){
    uint32_t k = f->superblock.s_log_block_size/4;
    uint32_t * blockList= NULL;
    blockList= new uint32_t [sizeof(buf)];
    uint32_t index;
    //if block is in the i_block array
    if (bNum < 12){
        blockList = i->i_block;
        goto direct;
    }
    else if (bNum < 12+k){
        if (i->i_block[12]==0){
            return false;
        }
        fetchBlock(f,i->i_block[12],buf);
        blockList= static_cast<uint32_t *>(buf);
        bNum= bNum-12;
        goto direct;
    }
    else if (bNum < (12+k+ (k*k)) ){
        if (i->i_block[13]==0){
            return false;
        }
        fetchBlock(f,i->i_block[13],buf);
        blockList = static_cast<uint32_t *>(buf);
        bNum= bNum-12-k;
        goto single;
    }
    else{
        if (i->i_block[14]==0){
            return false;
        }
        fetchBlock(f,i->i_block[14],buf);
        blockList = static_cast<uint32_t *>(buf);
        bNum = bNum-12-k-(k*k);

        index= (bNum) / (k*k);
        bNum = (bNum) % (k*k);
        if (blockList[index]==0){
            return false;
        }

        fetchBlock(f,blockList[index],buf);
        blockList= static_cast<uint32_t *>(buf);
        goto single;
    }

    single:
    index = (bNum)/k;
    bNum= (bNum) % k;

    if (blockList[index] ==0){
        return false;
    }

    fetchBlock(f,blockList[index],buf);
    blockList= static_cast<uint32_t *>(buf);

    direct:
    if (blockList[bNum]==0){
        return false;
    }

    fetchBlock(f,blockList[bNum],buf);
    return true;

}

struct Directory *openDir(struct  Ext2File *f, uint32_t iNum){
    Directory *directory=new Directory;
    directory->cursor=0;
    //opening directory with given number of inum
    directory->iNum=iNum;
    fetchInode(f,iNum,&directory->inode);

    directory->ptr = new uint8_t[f->superblock.s_log_block_size];
    directory->ext2File=f;
    return directory;
}


bool getNextDirent(struct Directory *d,uint32_t &iNum,char *name){
    while (d->cursor < d->inode.i_size){
        //calculate the block number
        int blockNum = d->cursor/d->ext2File->superblock.s_log_block_size;

        //calculate offset
        int offset = d->cursor - (blockNum * d->ext2File->superblock.s_log_block_size);  //changed

        fetchBlockFromFile(d->ext2File,&d->inode,blockNum,d->ptr);
        d->directoryEntry = (DirectoryEntry *) (d->ptr+offset);
        //taking the cursor to next directory entry
        d->cursor += d->directoryEntry->recLen;
        if (d->directoryEntry->iNum !=0 ){
            iNum= d->directoryEntry->iNum;

            for (int i = 0; i < d->directoryEntry->nameLen; i++) {
                name[i]= d->directoryEntry->name[i];
            }

            name[d->directoryEntry->nameLen]=0;
            return true;
        }
    }
    return false;
}


void rewindDir(struct Directory *d){
    d->cursor=0;
}

void closeDir(struct Directory *d){
    close(d->ext2File->partitionFile->f->fileDescripter);
}


uint32_t searchDir(struct Ext2File *f,uint32_t iNum,char *target){
    char name[256];
    Directory *directory;
    directory= openDir(f,iNum);
    uint32_t directoryInodeNum;
    while (getNextDirent(directory,directoryInodeNum,name)) {
           //return Inode num if target equals anme
        if (strcmp(target,name) ==0){
            return directoryInodeNum;
        }
    }
    return 0;
}


//provided algorithm
uint32_t traversePath(Ext2File *f,char *path){
    uint32_t  start =1;
    uint32_t  len =strlen(path);
    uint32_t iNum = 2;
    while (start < len && iNum != 0 ){
        uint32_t end = start+1;
        while (path[end] != 0 and path[end] != '/') {
            end++;
        }
        path[end] =0;
        iNum  =searchDir(f,iNum,path+start);
        start = end+1;
    }
    return iNum;
}


//Make a function to copy file from vdi to host
void copyFunction(char *vdiFileName, char *hostFilePath, Ext2File*f){

    char *vdiName = new char [256];
    vdiName = vdiFileName;

    uint8_t * dataBlock = NULL;
    dataBlock = new uint8_t [f->superblock.s_log_block_size];

    //traversing into the file
    uint32_t iNum = traversePath(f,vdiFileName);

    Inode inode;
    fetchInode(f,iNum,&inode);

    //destination
    int fileDirectory = open(hostFilePath,O_WRONLY|O_CREAT,0666);


    int remaining_bytes = inode.i_size, bNum=0;

    while (remaining_bytes > 0) {
        fetchBlockFromFile(f,&inode,bNum++,dataBlock);
        if (remaining_bytes >= f->superblock.s_log_block_size){

            write(fileDirectory,dataBlock,f->superblock.s_log_block_size);
            remaining_bytes -= f->superblock.s_log_block_size;
        }


        else{
            write(fileDirectory,dataBlock,remaining_bytes);
            remaining_bytes -= remaining_bytes;
        }
    }


    close (fileDirectory);
    delete [] dataBlock;


}



//Test functions
/*
void displayBuffer(char *buffer, int size) {
 for (int i = 0; i < size; ++i) {
        if (i % 16 == 0 && i != 0) {
            cout << "   ";

            for (int j = i - 16; j < i; ++j) {
                if ((int) buffer[j] >= 32 && (int) buffer[j] <= 126) {
                    cout << buffer[j];
                } else {
                    cout << " ";
                }
            }
            cout << endl;
        }
        uint8_t byte = buffer[i];
        cout << hex << uppercase << setfill('0') << setw(2) << (int) byte << " ";
    }

    if (size % 16 != 0) {
        for (int i = 0; i < (16 - (size % 16)) * 3; ++i) {
            std::cout << " ";
        }
    }
    std::cout << "   ";

    for (unsigned int i = size - (size - (size % 16) == size ? 16 : size % 16); i < size; ++i) {
        if ((int) buffer[i] >= 32 && (int) buffer[i] <= 126) {
            cout << buffer[i];
        } else {
            cout << " ";
        }
    }
    cout << std::endl;
    cout.flags(oldFlags);
}
}

void displayInode(Inode *inode){
    cout << inode->i_size << endl;
    cout << inode->i_blocks << endl;
    cout << inode->i_links_count << endl;
    cout << inode->i_flags <<endl;
    cout << inode->i_block[12] << endl;
    cout << inode->i_block[13] << endl;
    cout << inode->i_block[14] << endl;


*/


void displayFiles(Ext2File *f, uint32_t dNum){
    char *name = new char[256];
    Directory *directory = openDir(f, dNum);
    int i = 0;
    uint32_t iNum;

    while (getNextDirent(directory,iNum,name)){
            //omitting two dots, i counter helps track characters
        if (i>=2){
            Inode inode;
            fetchInode(f, iNum, &inode );
            cout<< name<< endl;

            if (inode.i_mode == 16877){
                displayFiles(f,iNum);
            }

        }

        i++;
    }

}



//display files in an ext2file system with file info
void displayFilesWithInfo(Ext2File *f, uint32_t dirNum){
    Directory *directory;
    directory = openDir(f, dirNum);
    char name[256];
    uint32_t iNum;
    uint32_t count = 0;
    string newfilepath = filepath;
    while (getNextDirent(directory, iNum, name)) {
        if (count >= 2) {
            Inode inode;
            fetchInode(f, iNum, &inode);
            filepath.append("/");
            filepath.append(name);


            cout << "File path : " << filepath << endl;
            cout << "File_Size : " << inode.i_size << endl;
            cout << "UID : " << inode.i_uid << endl;
            cout << "GID : " << inode.i_gid << endl;
            cout << "Links : " << inode.i_links_count << endl;
            cout << "Inum : " << iNum << endl;

            time_t accessTime = inode.i_mtime;
            string readableTime = ctime(&accessTime);
            //
            cout << "Last Access Time : " << readableTime << endl;

            if (inode.i_mode == 16877) {
                displayFilesWithInfo(f,iNum);
            }
            filepath = newfilepath;
        }
        count ++;
    }
    cout << endl;
}

