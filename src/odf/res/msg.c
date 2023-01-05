#include "string.h"
#include "odf/res/msg.h"
#include "odf/res/types/msg_def.h"

#include "odf/sys/memory.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/list.h"
#include "odf/sys/strings.h"

#include <stdlib.h>
#include <strings.h>

static OptionalPtr* readFileZeroTerminated(InMemoryFile* file);

static bool startsWith(char* str, char c) {
    for(size_t i = 0; i < strlen(str); i++) {
        if(str[i] == ' ' || str[i] == '\t')
            continue;
        if(str[i] == c)
            return true;
    }
    return false;
}


OptionalPtr* msgOpenFile(InMemoryFile* file) {
    OptionalPtr* optContent = readFileZeroTerminated(file);
    if(optionalIsEmpty(optContent)) return optContent;

    char* content = optionalGet(optContent);

    MsgFile* msgFile = memoryAllocateWithTag(sizeof(MsgFile), "odf/res/msg/MsgFile");

    List* lines = strSplitByLineBreak(content);

    for(size_t i = 0; i < listSize(lines); i++) {
        char* line = optionalGet(listGet(lines, i));
        if(strlen(line) == 0 || startsWith(line, '#'))
            continue;

        sscanf(
            line, "%s %d.%d",
            msgFile->magic,
            &(msgFile->majorVersion),
            &(msgFile->minorVersion)
        );

        break;
    }

    memoryRelease(content);

    for(size_t i = 0; i < listSize(lines); i++) {
        char* line = optionalGet(listGet(lines, i));
        memoryRelease(line);
    }
    listDelete(lines);

    return optionalOf(msgFile);
}

void msgCloseFile(MsgFile* msgFile) {
    memoryRelease(msgFile);
}




// Private functions

static OptionalPtr* readFileZeroTerminated(InMemoryFile* file) {
    uint32_t size = inMemFileSize(file);
    if(size < 6) return optionalEmpty("Could not parse MSG file");

    OptionalPtr* optContent = inMemFileRead(file, size);
    if(optionalIsEmpty(optContent)) return optContent;

    char* content = optionalGet(optContent);
    char* zeroTerminatedContent = memoryAllocate((size+1)*sizeof(char));
    strncpy(zeroTerminatedContent, content, size);
    zeroTerminatedContent[size] = 0;

    memoryRelease(content);

    return optionalOf(zeroTerminatedContent);
}
