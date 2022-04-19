#ifndef DIRECTORYENTRY_H_INCLUDED
#define DIRECTORYENTRY_H_INCLUDED

struct DirectoryEntry {
uint32_t iNum;
uint16_t recLen;
uint8_t nameLen;
uint8_t fileType;
uint8_t name[1];

}__attribute__((packed));

#endif // DIRECTORYENTRY_H_INCLUDED
