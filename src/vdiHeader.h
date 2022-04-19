#ifndef VDIHEADER
#define VDIHEADER


struct vdiheader{
char imageName[64];
uint32_t signature;
uint32_t version;
uint32_t headerSize;
uint32_t imageType;
uint32_t flag;
char comment[256];
uint32_t offBlock;
uint32_t offData;
uint32_t cylinders;
uint32_t heads;
uint32_t sectors;
uint32_t sectorSizeB;
uint32_t dummy;
uint64_t diskSize;
uint32_t blockSize;
uint32_t blockExtra;
uint32_t blocks;
uint32_t allocatedBlocks;
char uuidCreate[16];
char uuidModify[16];
char uuidLinkage[16];
char uuidParentModify[16];
}__attribute__((packed));

#endif // VDIHEADER
