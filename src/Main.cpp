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

int main(){

        cout << "vdiFile path: " << endl;
        char *vdiFileName = new char [256];
        cin >> vdiFileName;
        cout<<endl;
        Ext2File *ext2File= ext2Open(vdiFileName,0);
        cout << "All files in vdi:" << endl;
        cout<< endl;
        displayFiles(ext2File,2);
        cout<<endl;

        cout << "Files with info" << endl;

        displayFilesWithInfo(ext2File,2);

        cout << "Path to the file that needs copied over:" << endl;
        char *vdiFile = new char [256];
        cin >> vdiFile;
        cout << "Host file path:" << endl;
        char *hostFilePath = new char [256];
        cin >> hostFilePath;
        copyFunction(vdiFile,hostFilePath, ext2File);
        cout << "File copied over" << endl;

}


