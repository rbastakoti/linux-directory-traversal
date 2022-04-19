#ifndef PARTITIONENTRY_H_INCLUDED
#define PARTITIONENTRY_H_INCLUDED


struct PartitionEntry{
uint8_t bootStatus;
uint8_t firstChs[3];
uint8_t descriptor;
uint8_t lastChs[3];
uint32_t blocking;
uint32_t numberOfSector;
}__attribute__((packed));


#endif // PARTITIONENTRY_H_INCLUDED
