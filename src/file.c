#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "system/optional.h"
#include "system/memory.h"
#include "system/strings.h"

OptionalPtr* fileOpen(char* filePath, char* modifiers) {
    FILE* stream = fopen(filePath, modifiers);
    if(stream == NULL) {
        return optionalEmpty("file.c:fileOpen - Could not open file");
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

    if(length == -1L) return optionalEmpty("file.c:fileReadAll - File lenght is invalid");

    void* optionalContent = memoryAllocate(length);
    if(optionalIsEmpty(optionalContent)) return optionalContent;
    char* content = optionalGet(optionalContent);

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
    OptionalPtr* optionalBytes = memoryAllocate(length);
    if(optionalIsEmpty(optionalBytes)) return optionalBytes;

    char* bytes = optionalGet(optionalBytes);

    uint32 objects = fread(bytes, length, 1, stream);
    if(objects != 1) {
        memoryRelease(bytes);
        return optionalEmpty("file.c:fileReadBytes - Could not read %d byte(s). End of file", length);
    }

    return optionalOf(bytes);
}

OptionalUInt8* fileReadLittleEndianUInt8(FILE* stream) {
    uint8 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt8(value);
}

OptionalUInt16* fileReadLittleEndianUInt16(FILE* stream) {
    uint16 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt16(value);
}

OptionalUInt32* fileReadLittleEndianUInt32(FILE* stream) {
    uint32 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt32(value);
}

OptionalUInt64* fileReadLittleEndianUInt64(FILE* stream) {
    uint64 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt64(value);
}





OptionalInt8* fileReadLittleEndianInt8(FILE* stream) {
    int8 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalInt8(value);
}

OptionalInt16* fileReadLittleEndianInt16(FILE* stream) {
    int16 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalInt16(value);
}

OptionalInt32* fileReadLittleEndianInt32(FILE* stream) {
    int32 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalInt32(value);
}

OptionalInt64* fileReadLittleEndianInt64(FILE* stream) {
    int64 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalInt64(value);
}














OptionalUInt8* fileReadBigEndianUInt8(FILE* stream) {
    uint8 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isLittleEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt8(value);
}

OptionalUInt16* fileReadBigEndianUInt16(FILE* stream) {
    uint16 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isLittleEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt16(value);
}

OptionalUInt32* fileReadBigEndianUInt32(FILE* stream) {
    uint32 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isLittleEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt32(value);
}

OptionalUInt64* fileReadBigEndianUInt64(FILE* stream) {
    uint64 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isLittleEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt64(value);
}





OptionalInt8* fileReadBigEndianInt8(FILE* stream) {
    int8 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isLittleEndian()) reverseEndianness(&value, sizeof(value));

    return optionalInt8(value);
}

OptionalInt16* fileReadBigEndianInt16(FILE* stream) {
    int16 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isLittleEndian()) reverseEndianness(&value, sizeof(value));

    return optionalInt16(value);
}

OptionalInt32* fileReadBigEndianInt32(FILE* stream) {
    int32 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isLittleEndian()) reverseEndianness(&value, sizeof(value));

    return optionalInt32(value);
}

OptionalInt64* fileReadBigEndianInt64(FILE* stream) {
    int64 value;

    uint32 objects = fread(&value, sizeof(value), 1, stream);
    if(objects != 1)
        return optionalEmpty("file.c:fileReadBytes - Could not read %d bit int. End of file", sizeof(value)*8);

    if(isLittleEndian()) reverseEndianness(&value, sizeof(value));

    return optionalInt64(value);
}
