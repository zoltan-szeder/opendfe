#include <string.h>
#include <stdarg.h>

#include "odf/sys/inmemoryfile.h"
#include "odf/sys/types.h"
#include "stdlib.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"
#include "odf/sys/strings.h"
#include "odf/sys/log.h"

struct InMemoryFile {
    uint32_t length;
    uint64_t pos;
    char* content;
};

OptionalPtr* memFileCreate(char* content, uint32_t length) {
    logTrace("Creating an in memory file with %d byte(s)", length);
    InMemoryFile* file = memoryAllocateWithTag(sizeof(InMemoryFile), "odf/sys/inmemoryfile/InMemoryFile");

    file->content = content;
    file->length = length;
    file->pos = 0;

    return optionalOf(file);
}

int32_t memFileSeek(InMemoryFile* file, int64_t offset, int32_t origin) {
    logTrace(
        "Seeking from position %d, moving %d from %s",
        file->pos,
        offset,
        origin == SEEK_CUR ? "SEEK_CURR" : (origin == SEEK_SET ? "SEEK_SET" : "SEEK_END")
    );
    uint64_t currentPosition;
    if (origin == SEEK_CUR) currentPosition = file->pos;
    else if (origin == SEEK_SET) currentPosition = 0;
    else currentPosition = file->length;

    currentPosition += offset;
    if(currentPosition > file->length) {
        logTrace("Could not find position");
        return 1;
    }

    file->pos = currentPosition;
    logTrace("New position in file is %d", file->pos);
    return 0;
}

void inMemFileDelete(InMemoryFile* file) {
    logTrace("Releasing InMemoryFile %p", file);
    memoryRelease(file->content);
    memoryRelease(file);
}

uint64_t inMemFileSize(InMemoryFile* file) {
    return file->length;
}

bool inMemFileOverEOF(InMemoryFile* file, uint64_t length) {
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

OptionalPtr* inMemFileRead(InMemoryFile* file, uint32_t length) {
    logTrace("Reading %d byte(s)", length);
    if(inMemFileOverEOF(file, length)) {
        logTrace(
            "inMemFileRead - Could not read %d bytes from %d offset in a %d long file",
            length, file->pos, file->length
        );
        return optionalEmpty(
            "inMemFileRead - Could not read %d bytes from %d offset in a %d long file",
            length, file->pos, file->length
        );
    }

    uint8_t* bytes = memoryAllocateWithTag(length, "odf/sys/inmemoryfile/InMemoryFile/content");
    memcpy(bytes, file->content + file->pos, length);
    file->pos += length;

    return optionalOf(bytes);
}
