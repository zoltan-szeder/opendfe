#include <stdio.h>
#include <stdlib.h>

#include "odf/sys/file.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"
#include "odf/sys/strings.h"

OptionalPtr* fileOpen(char* filePath, char* modifiers) {
    FILE* stream = fopen(filePath, modifiers);
    if(stream == NULL) {
        return optionalEmpty("fileOpen - Could not open file \"%s\"", filePath);
    } else {
        return optionalOf(stream);
    }
}

OptionalPtr* fileReadAll(char* filePath) {
    void* optionalStream = fileOpen(filePath, "rb");
    if(optionalIsEmpty(optionalStream)) {
        return optionalStream;
    }
    FILE* stream = optionalGet(optionalStream);

    fseek(stream, 0, SEEK_END);
    long length = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    if(length == -1L) return optionalEmpty("fileReadAll - File lenght is invalid");

    char* content = memoryAllocateWithTag(length, "odf/sys/file/fileReadAll");

    fread(content, length, 1, stream);
    content[length-1] = (char) 0;
    fclose(stream);

    return optionalOf(content);
}


OptionalPtr* fileReadStruct(FILE* stream, const char* format) {
    size_t size = getBlockLenghtFromFormat(format);

    OptionalPtr* optional = fileReadBytes(stream, size);
    if(optionalIsEmpty(optional)) return optional;

    return optionalOf(
        modifyEndiannessOfStruct(
            optionalGet(optional),
            format
        )
    );
}


OptionalPtr* fileReadBytes(FILE* stream, size_t length) {
    char* bytes = memoryAllocateWithTag(length, "odf/sys/file/fileReadBytes");
    memoryTag(bytes, "odf/sys/file/FileContent");

    uint32_t objects = fread(bytes, length, 1, stream);
    if(objects != 1) {
        memoryRelease(bytes);
        return optionalEmpty("fileReadBytes - Could not read %d byte(s). End of file", length);
    }

    return optionalOf(bytes);
}
