#include "odf/res/3do.h"
#include "odf/res/types/3do_def.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"
#include "odf/sys/strings.h"
#include <string.h>


OptionalOf(3DOFile)* tdoOpenFile(InMemoryFile* file) {
    size_t fileSize = inMemFileSize(file);
    if(fileSize == 0) {
        return optionalEmpty("Could not read file");
    }

    OPTIONAL_ASSIGN_OR_PASSTROUGH(char*, content, inMemFileRead(file, inMemFileSize(file)));
    
    TDOFile* tdo = memoryAllocateWithTag(sizeof(TDOFile), "TDOFile");

    return optionalOf(tdo);
}

void tdoCloseFile(TDOFile* tdoFile) {
    memoryRelease(tdoFile);
}
