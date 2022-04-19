#ifndef PARTITION_H_INCLUDED
#define PARTITION_H_INCLUDED
#include "partitionEntry.h"

struct PartitionFile{
struct vdiFile *f;
PartitionEntry partitionEntry;
}__attribute__((packed));



#endif // PARTITION_H_INCLUDED
