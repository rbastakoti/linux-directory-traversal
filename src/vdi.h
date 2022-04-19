#ifndef VDI
#define VDI




struct vdiFile {
int fileDescripter;
vdiheader header;
ssize_t cursor;
}__attribute__((packed));


#endif


