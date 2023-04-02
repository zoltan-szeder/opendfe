#include <stdlib.h>
#include <string.h>

#include "odf/res/rle.h"
#include "odf/sys/memory.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/optional.h"
#include "odf/sys/log.h"
#include "odf/sys/list.h"

static void rleCopyBlock(uint8_t* dest, uint8_t destOffset, uint8_t* src, uint8_t srcOffset, uint8_t length);
static void rleSkipBlock(uint8_t* dest, uint8_t destOffset, uint8_t length, uint8_t color);

static OptionalOf(ListOf(uint8_t*))* rleReadOffsets(InMemoryFile* file, RLEConfig* config);
static OptionalOf(uint8_t*)* rleReadData(InMemoryFile* file, ListOf(uint32_t*)* offsets, RLEConfig* config);
static OptionalOf(uint8_t*)* rleReadSequence(InMemoryFile* file, RLEConfig* config);
static OptionalOf(uint8_t*)* rleReadBlock(InMemoryFile* file, RLEConfig* config, uint8_t controlByte);
static uint8_t rleGetBlockSize(uint8_t controlByte);

static void rleOffsetsDelete(ListOf(uint32_t*)* offsets);

OptionalOf(uint8_t*)* rleReadFile(InMemoryFile* file, RLEConfig* config){
    OPTIONAL_ASSIGN_OR_PASSTROUGH(
        ListOf(uint32_t*)*, offsets, rleReadOffsets(file, config);
    );

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        uint8_t*, data, rleReadData(file, offsets, config), {
        rleOffsetsDelete(offsets);
    });
    rleOffsetsDelete(offsets);

    return optionalOf(data);
}

static OptionalOf(ListOf(uint8_t*))* rleReadOffsets(InMemoryFile* file, RLEConfig* config){
    ListOf(uint32_t*)* offsets = listCreate(config->offsetCount);

    for(uint32_t i = 0; i < config->offsetCount; i++) {
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint32_t*, offset, inMemFileReadStruct(file, "%l4"), {
                rleOffsetsDelete(offsets);
            }
        );
        *offset = *offset + config->fileOffset;
        logTrace("Found offset %x", *offset);
        listAppend(offsets, offset);
    }

    return optionalOf(offsets);
}

static OptionalOf(uint8_t*)* rleReadData(InMemoryFile* file, ListOf(uint32_t*)* offsets, RLEConfig* config){
    uint32_t dataSize = config->offsetCount * config->sequenceSize;
    uint8_t* data = memoryAllocateWithTag(dataSize * sizeof(uint8_t), "Data");
    uint8_t dataOffset = 0;
    forEachListItem(uint32_t*, offset, offsets, {
        int32_t location = memFileSeek(file, *offset, SEEK_SET);
        if(location != 0) {
            memoryRelease(data);
            return optionalEmpty("The sequence offset (%d) is outside the file's boundaries", *offset);
        }
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint8_t*, sequence, rleReadSequence(file, config), {
                memoryRelease(data);
            }
        );

        memcpy(data + dataOffset, sequence, config->sequenceSize);
        memoryRelease(sequence);
        dataOffset += config->sequenceSize;
    });

    return optionalOf(data);
}

static OptionalOf(uint8_t*)* rleReadSequence(InMemoryFile* file, RLEConfig* config) {
    uint8_t* sequence = memoryAllocateWithTag(
        config->sequenceSize*sizeof(uint8_t), "Sequence");

    uint32_t sequenceOffset = 0;
    while(sequenceOffset < config->sequenceSize) {
        uint8_t* controlByte = optionalGet(inMemFileRead(file, 1));
        memoryTag(controlByte, "ControlByte");

        logTrace("Found control byte: %x", *controlByte);
        uint8_t blockSize = rleGetBlockSize(*controlByte);
        if(blockSize > config->sequenceSize - sequenceOffset){
            memoryRelease(sequence);
            OptionalPtr* opt =  (void*) optionalEmpty(
                "The control byte contains lenght (%u) "
                "larger than the sequence (%u) "
                "from offset (%u)",
                *controlByte, config->sequenceSize, sequenceOffset);
            memoryRelease(controlByte);
            return opt;
        }
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint8_t*, block, rleReadBlock(file, config, *controlByte), {
                memoryRelease(controlByte);
                memoryRelease(sequence);
            }
        );
        memcpy(sequence + sequenceOffset, block, blockSize);
        sequenceOffset += blockSize;
        memoryRelease(controlByte);
        memoryRelease(block);
    }
    return optionalOf(sequence);
}

static OptionalOf(uint8_t*)* rleReadBlock(InMemoryFile* file, RLEConfig* config, uint8_t controlByte) {
    uint8_t blockSize = rleGetBlockSize(controlByte);
    if(controlByte > 128) {
        uint8_t value = 0;
        if(config->type == RLE1) {
            OPTIONAL_ASSIGN_OR_PASSTROUGH(
                uint8_t*, v, inMemFileRead(file, 1)
            );
            value = *v;
            memoryRelease(v);
        }
        uint8_t* block = memoryAllocateWithTag(blockSize, "RLE0Background");
        memset(block, value, blockSize);
        return optionalOf(block);
    }
    return inMemFileRead(file, blockSize);
}

static uint8_t rleGetBlockSize(uint8_t controlByte) {
    if(controlByte > 128) {
        return controlByte - 128;
    }
    return controlByte;
}

static void rleOffsetsDelete(ListOf(uint32_t*)* offsets) {
    forEachListItem(uint32_t*, offset, offsets, {
        memoryRelease(offset);
    })
    listDelete(offsets);
}

uint8_t* rle0Decompress(uint8_t* src, int32_t length, int32_t width, int32_t height) {
    uint8_t* dest = memoryAllocateWithTag(width*height*sizeof(uint8_t), "odf/res/ldf/rle0Decompress");

    int32_t di = 0;
    int32_t si = 0;
    while(si < length) {
        int32_t blockSize = src[si];
        si++;

        if(blockSize <= 128) {
            rleCopyBlock(dest, di, src, si, blockSize);
            si += blockSize;
            di += blockSize;
        } else {
            blockSize -= 128;
            char color = '\x00';

            rleSkipBlock(dest, di, blockSize, color);
            di += blockSize;
        }
    }

    return dest;
}

uint8_t* rle1Decompress(uint8_t* src, int32_t length, int32_t width, int32_t height) {
    uint8_t* dest = memoryAllocateWithTag(width*height*sizeof(uint8_t), "odf/res/ldf/rle0Decompress");

    int32_t di = 0;
    int32_t si = 0;
    while(si < length) {
        int32_t blockSize = src[si];
        si++;

        if(blockSize <= 128) {
            rleCopyBlock(dest, di, src, si, blockSize);
            si += blockSize;
            di += blockSize;
        } else {
            blockSize -= 128;
            char color = src[si];
            si++;

            rleSkipBlock(dest, di, blockSize, color);
            di += blockSize;
        }
    }

    return dest;
}


static void rleCopyBlock(uint8_t* dest, uint8_t destOffset, uint8_t* src, uint8_t srcOffset, uint8_t length) {
    memcpy(dest + destOffset, src + srcOffset, length);
}

static void rleSkipBlock(uint8_t* dest, uint8_t destOffset, uint8_t length, uint8_t color) {
    for(uint8_t i = 0; i < length; i++) {
        dest[destOffset+i] = color;
    }
}
