#include "string.h"
#include "odf/res/msg.h"
#include "odf/res/types/msg_def.h"

#include "odf/sys/memory.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/list.h"
#include "odf/sys/strings.h"
#include "odf/sys/log.h"

#include <stdlib.h>
#include <strings.h>

static OptionalPtr* readFileZeroTerminated(InMemoryFile* file);
static bool isEmpty(char* line);
static bool startsWith(char* str, char c);

static size_t findNextNonEmptyLine(List* lines, size_t cursor);
static void listDeepDeleteStringList(List* lines);

static OptionalPtr* msgReadVersion(List* lines, size_t* i);
static OptionalPtr* msgReadMsgList(List* lines, size_t* i);
static OptionalPtr* msgReadMessage(List* lines, size_t* i);


OptionalPtr* msgOpenFile(InMemoryFile* file) {
    OPTIONAL_ASSIGN_OR_PASSTROUGH(
        char*, content, readFileZeroTerminated(file)
    )

    List* lines = strSplitByLineBreak(content);
    memoryRelease(content);

    size_t i = 0;
    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        MsgFile*, msgFile, msgReadVersion(lines, &i),
        {listDeepDeleteStringList(lines);}
    );

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        size_t*, msgCountPtr, msgReadMsgList(lines, &i),
        {
            listDeepDeleteStringList(lines);
            memoryRelease(msgFile);
        }
    );
    size_t msgCount = *msgCountPtr;
    memoryRelease(msgCountPtr);

    msgFile->messages = listCreate(msgCount);

    for(size_t msgIndex = 0; msgIndex <  msgCount; msgIndex++) {
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            Msg*, msg, msgReadMessage(lines, &i),
            {
                listDeepDeleteStringList(lines);
                msgCloseFile(msgFile);
            }
        )
        listPut(msgFile->messages, msgIndex, msg);
    }

    listDeepDeleteStringList(lines);

    return optionalOf(msgFile);
}



OptionalPtr* msgGet(MsgFile* file, size_t index) {
    List* msgs = file->messages;
    for(size_t i = 0; i < listSize(msgs); i++) {
        OptionalPtr* optMsg = listGet(msgs, i);
        if(optionalIsEmpty(optMsg)) {
            optionalDelete(optMsg);
            continue;
        }

        Msg* msg = optionalGet(optMsg);

        if(msg->index == index) {
            return optionalOf(msg);
        }
    }

    return optionalEmpty("msgGet - Could not find message with index %ld", index);
}

void msgCloseFile(MsgFile* msgFile) {
    List* msgs = msgFile->messages;
    for(size_t i = 0; i < listSize(msgs); i++) {
        Msg* msg =  optionalGet(listGet(msgs, i));
        memoryRelease(msg->value);
        memoryRelease(msg);
    }
    listDelete(msgFile->messages);
    memoryRelease(msgFile);
}

// Private functions
void msgPrint(FILE* stream, MsgFile* msgFile){
    fprintf(stream, "%s %d.%d\n", msgFile->magic, msgFile->majorVersion, msgFile->minorVersion);

    List* msgs = msgFile->messages;
    size_t msgCount = listSize(msgs);

    fprintf(stream, "MSGS %ld\n", msgCount);
    for(size_t i = 0; i < msgCount; i++) {
        OptionalPtr* optMsg = listGet(msgs, i);
        if(optionalIsEmpty(optMsg)) {
            fprintf(stream, "# MISSING");
        } else {
            Msg* msg = optionalGet(optMsg);
            fprintf(stream, "\t%ld\t%ld:\t\"%s\"\n", msg->index, msg->priority, msg->value);
        }
    }
    fprintf(stream, "END\n");
}

static OptionalPtr* msgReadVersion(List* lines, size_t* i) {
    size_t lineCount = listSize(lines);

    *i = findNextNonEmptyLine(lines, 0);
    if(lineCount <= *i) {
        return optionalEmpty("msgOpenFile - Unexpected EOF before MSG version");
    }

    MsgFile* msgFile = memoryAllocateWithTag(sizeof(MsgFile), "odf/res/msg/MsgFile");
    {
        char* line = optionalGet(listGet(lines, *i));
        int result = sscanf(
            line, "%s %d.%d",
            msgFile->magic,
            &(msgFile->majorVersion),
            &(msgFile->minorVersion)
        );

        if(result != 3) {
            memoryRelease(msgFile);
            return optionalEmpty("Could not parse MSG version number from \"%s\"", line);
        }

        logDebug("Found MSG file with version %d.%d", msgFile->majorVersion, msgFile->minorVersion);

        *i = *i+1;
    }

    return optionalOf(msgFile);
}

static OptionalPtr* msgReadMsgList(List* lines, size_t* i) {
    size_t lineCount = listSize(lines);

    *i = findNextNonEmptyLine(lines, *i);
    if(lineCount <= *i)
        return optionalEmpty("msgOpenFile - Unexpected EOF before MSGS count");

    char* line = optionalGet(listGet(lines, *i));
    logDebug("Parsing line: %s", line);

    char msgs[5];
    size_t* msgCount = memoryAllocate(sizeof(size_t));

    if(sscanf(line, "%s %ld", msgs, msgCount) != 2) {
        memoryRelease(msgCount);
        return optionalEmpty("Could not parse MSG count from: \"%s\"", line);
    }

    logDebug("%s Count: %ld", msgs, *msgCount);

    *i = *i + 1;
    return optionalOf(msgCount);
}

static OptionalPtr* msgReadMessage(List* lines, size_t* i){
    size_t lineCount = listSize(lines);
    *i = findNextNonEmptyLine(lines, *i);
    if(lineCount <= *i) {
        return optionalEmpty("msgOpenFile - Unexpected EOF before all messages would be read");
    }

    char* line = optionalGet(listGet(lines, *i));

    Msg* msg = memoryAllocate(sizeof(Msg));
    msg->value = memoryAllocate(64*sizeof(char));
    int result = sscanf(
        line,
        "%ld %ld: \"%[^\"]\"",
        &(msg->index),
        &(msg->priority),
        msg->value
    );

    if(result != 3) {
        memoryRelease(msg->value);
        memoryRelease(msg);
        return optionalEmpty("Could not parse MSG message from line: \"%s\"", line);
    }

    logDebug("Found message %ld with priority %ld: \"%s\"", msg->index, msg->priority, msg->value);
    *i = *i + 1;
    return optionalOf(msg);
}

static OptionalPtr* readFileZeroTerminated(InMemoryFile* file) {
    uint32_t size = inMemFileSize(file);
    if(size < 6) return optionalEmpty("readFileZeroTerminated - Could not parse MSG file");

    OptionalPtr* optContent = inMemFileRead(file, size);
    if(optionalIsEmpty(optContent)) return optContent;

    char* content = optionalGet(optContent);
    char* zeroTerminatedContent = memoryAllocate((size+1)*sizeof(char));
    strncpy(zeroTerminatedContent, content, size);
    zeroTerminatedContent[size] = 0;

    memoryRelease(content);

    return optionalOf(zeroTerminatedContent);
}

static bool isEmpty(char* line) {
    return strlen(line) == 0 || startsWith(line, '#');
}

static bool startsWith(char* str, char c) {
    for(size_t i = 0; i < strlen(str); i++) {
        if(str[i] == ' ' || str[i] == '\t')
            continue;
        if(str[i] == c)
            return true;
        return false;
    }
    return false;
}

static size_t findNextNonEmptyLine(List* lines, size_t cursor) {
    size_t size = listSize(lines);
    for(size_t i = cursor; i < size; i++) {
        char* line = optionalGet(listGet(lines, i));
        if(!isEmpty(line)) {
            return i;
        }
    }

    return size;
}

static void listDeepDeleteStringList(List* lines){
    for(size_t i = 0; i < listSize(lines); i++) {
        char* line = optionalGet(listGet(lines, i));
        memoryRelease(line);
    }

    listDelete(lines);
}

