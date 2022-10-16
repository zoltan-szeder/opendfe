#include <string.h>
#include <stdarg.h>

#include "odf/sys/inmemoryfile.h"
#include "odf/sys/types.h"
#include "stdlib.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"
#include "odf/sys/strings.h"

struct InMemoryFile {
    uint32 length;
    uint64 pos;
    char* content;
};

OptionalPtr* memFileCreate(char* content, uint32 length) {
    InMemoryFile* file = memoryAllocateWithTag(sizeof(InMemoryFile), "odf/sys/inmemoryfile/InMemoryFile");

    file->content = content;
    file->length = length;
    file->pos = 0;

    return optionalOf(file);
}

int32 memFileSeek(InMemoryFile* file, int64 offset, int32 origin) {
    uint64 currentPosition;
    if (origin == SEEK_CUR) currentPosition = file->pos;
    else if (origin == SEEK_SET) currentPosition = 0;
    else currentPosition = file->length;

    currentPosition += offset;
    if(currentPosition > file->length)
        return 1;

    file->pos = currentPosition;
    return 0;
}

void inMemFileDelete(InMemoryFile* file) {
    memoryRelease(file->content);
    memoryRelease(file);
}

uint64 inMemFileSize(InMemoryFile* file) {
    return file->length;
}

bool inMemFileOverEOF(InMemoryFile* file, uint64 length) {
    return file->pos + length > file->length;
}

OptionalPtr* inMemFileReadStruct(InMemoryFile* file, const char* format) {
    size_t size = getBlockLenghtFromFormat(format);

    OptionalPtr* optional = inMemFileRead(file, size);
    if(optionalIsEmpty(optional)) return optional;

    return optionalOf(
        modifyEndiannessOfStruct(
            optionalGet(optional),
            format
        )
    );
}

OptionalPtr* inMemFileRead(InMemoryFile* file, uint32 length) {
    if(inMemFileOverEOF(file, length))
        return optionalEmpty(
            "odf/sys/inmemoryfile.c:inMemFileRead - Could not read %d bytes from %d offset in a %d long file",
            length, file->pos, file->length
        );

    uint8* bytes = memoryAllocateWithTag(length, "odf/sys/inmemoryfile/InMemoryFile/content");
    memcpy(bytes, file->content + file->pos, length);
    file->pos += length;

    return optionalOf(bytes);
}
