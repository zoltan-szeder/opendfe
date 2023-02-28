#include "odf/res/fnt.h"
#include "odf/res/types/fnt_def.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"
#include "odf/sys/list.h"


static OptionalOf(ListOf(FNTChar*)*)* fntReadChars(InMemoryFile* file, FNTHeader* header);
static void fntCloseChars(ListOf(FNTChar*)* fntChars);


OptionalOf(FNTFile*)* fntRead(InMemoryFile* file) {
    FNTFile* fntFile = memoryAllocate(sizeof(FNTFile));

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        FNTHeader*, header, inMemFileReadStruct(file, FNT_HEADER_FORMAT),
        {
            fntClose(fntFile);
        }
    )
    memoryTag(header, "odf/res/fnt/FNTHeader");
    fntFile->header = header;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        ListOf(FNTChar*)*, chars, fntReadChars(file, header),
        {
            fntClose(fntFile);
        }
    )
    memoryTag(chars, "odf/res/fnt/FNTChars");
    fntFile->chars = chars;

    return optionalOf(fntFile);
}

void fntClose(FNTFile* fntFile) {
    if(!fntFile) return;

    if(fntFile->chars)
        fntCloseChars(fntFile->chars);

    if(fntFile->header)
        memoryRelease(fntFile->header);

    memoryRelease(fntFile);
}

static void fntCloseChars(ListOf(FNTChar*)* fntChars) {
    forEachListItem(
        FNTChar*, fntChar, fntChars,
        memoryRelease(fntChar->data);
        memoryRelease(fntChar);
    )
    listDelete(fntChars);
}

static OptionalOf(ListOf(FNTChar*)*)* fntReadChars(InMemoryFile* file, FNTHeader* header) {
    uint8_t charCount = header->lastChar - header->firstChar + 1;
    ListOf(FNTChar*)* fntChars = listCreate(charCount);
    for(uint8_t i = 0; i < charCount; i++) {
        FNTChar* fntChar = memoryAllocateWithTag(sizeof(FNTChar), "odf/res/fnt/FNTChar");

        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint8_t*, width, inMemFileReadStruct(file, "%c1"),
            {
                memoryRelease(fntChar);
                fntCloseChars(fntChars);
            }
        )

        fntChar->width = *width;
        memoryRelease(width);

        uint16_t dataSize = header->height * fntChar->width;
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint8_t*, data, inMemFileRead(file, dataSize),
            {
                memoryRelease(fntChar);
                fntCloseChars(fntChars);
            }
        )

        fntChar->data = data;

        listAppend(fntChars, fntChar);

    }
    return optionalOf(fntChars);
}
