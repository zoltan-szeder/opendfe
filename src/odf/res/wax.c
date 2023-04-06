#include "odf/res/wax.h"
#include "odf/res/types/wax_def.h"

#include <string.h>

#include "odf/sys/list.h"
#include "odf/sys/memory.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/strings.h"
#include "odf/sys/optional.h"
#include "odf/sys/log.h"
#include "odf/res/rle.h"

static OptionalOf(ListOf(WAX*)*)* waxReadWaxList(InMemoryFile* file);
static OptionalOf(WAX*)* waxReadWax(InMemoryFile* file);
static OptionalOf(ListOf(WAXSequence*)*)* waxReadSequences(InMemoryFile* file);
static OptionalOf(WAXSequence*)* waxReadSequence(InMemoryFile* file);
static OptionalOf(ListOf(WAXFrame*)*)* waxReadFrames(InMemoryFile* file);
static OptionalOf(WAXFrame*)* waxReadFrame(InMemoryFile* file);
static OptionalOf(WAXCell*)* waxReadCell(InMemoryFile* file);

static void waxCloseWaxList(ListOf(WAX*)* waxList);
static void waxCloseWax(WAX* wax);
static void waxCloseSequences(ListOf(WAXSequence*)* sequences);
static void waxCloseSequence(WAXSequence* sequence);
static void waxCloseFrames(ListOf(WAXFrame*)* frames);
static void waxCloseFrame(WAXFrame* frame);
static void waxCloseCell(WAXCell* frame);

static void waxPrint(WAXFile* waxFile);
static void waxPrintHeader(WAXHeader* header);
static void waxPrintWAX(WAX* wax);
static void waxPrintSeq(WAXSequence* seq);
static void waxPrintFrame(WAXFrame* frame);
static void waxPrintCell(WAXCell* cell);

OptionalOf(WAXFile*)* waxReadInMemoryFile(InMemoryFile* file) {
    WAXFile* waxFile = memoryAllocateWithTag(sizeof(WAXFile), "WAXFile");
    waxFile->header = NULL;
    waxFile->waxList = NULL;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        WAXHeader*, header, inMemFileReadStruct(file, WAX_HEADER_FORMAT),
        {waxClose(waxFile);}
    );
    memoryTag(header, "WAXHeader");
    waxFile->header = header;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        ListOf(WAX*)*, waxList, waxReadWaxList(file),
        {waxClose(waxFile);}
    );
    waxFile->waxList = waxList;

    waxPrint(waxFile);
    return optionalOf(waxFile);
}


static OptionalOf(ListOf(WAX*)*)* waxReadWaxList(InMemoryFile* file){
    ListOf(WAX*)* waxList = listCreate(0);
    logTrace("Allocating WAX List at %p", waxList);

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        uint32_t*, waxOffsets, inMemFileReadStruct(file, UINT32_X_32),
        {waxCloseWaxList(waxList);}
    );

    logTrace("wax_offsets:");
    for(uint32_t i = 0; i < 32; i++) {
        uint32_t waxOffset = waxOffsets[i];
        if(!waxOffset) break;

        memFileSeek(file, waxOffset, SEEK_SET);
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            WAX*, wax, waxReadWax(file),
            {waxCloseWaxList(waxList);}
        );
        listAppend(waxList, wax);
    }

    memoryRelease(waxOffsets);
    return optionalOf(waxList);
}

static OptionalOf(WAX*)* waxReadWax(InMemoryFile* file){
    WAX* wax = memoryAllocateWithTag(sizeof(WAX), "WAX");
    logTrace("Allocating WAX at %p", wax);
    wax->sequences = NULL;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        void*, header, inMemFileReadStruct(file, WAX_FORMAT),
        {waxCloseWax(wax);}
    )
    size_t length = getBlockLenghtFromFormat(WAX_FORMAT);
    memcpy(wax, header, length);
    memoryRelease(header);

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        ListOf(WAXSequence*)*, sequences, waxReadSequences(file),
        {waxCloseWax(wax);}
    );
    wax->sequences = sequences;

    return optionalOf(wax);
}

static OptionalOf(ListOf(WAXSequence*)*)* waxReadSequences(InMemoryFile* file){
    ListOf(WAXSequence*)* sequences = listCreate(0);
    memoryTag(sequences, "SEQS");
    logTrace("Allocating Sequence List at %p", sequences);


    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        uint32_t*, seqOffsets, inMemFileReadStruct(file, UINT32_X_32),
        {waxCloseSequences(sequences);}
    );

    logTrace("seq_offsets:");
    for(uint32_t i = 0; i < 32; i++) {
        uint32_t seqOffset = seqOffsets[i];
        if(!seqOffset) break;

        logTrace("- %d", seqOffset);
        memFileSeek(file, seqOffset, SEEK_SET);
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            WAXSequence*, sequence, waxReadSequence(file),
            {waxCloseSequences(sequences);}
        );
        listAppend(sequences, sequence);
    }

    memoryRelease(seqOffsets);

    return optionalOf(sequences);

}

static OptionalOf(WAXSequence*)* waxReadSequence(InMemoryFile* file) {
    WAXSequence* seq = memoryAllocateWithTag(sizeof(WAXSequence), "WAXSequence");
    logTrace("Allocating Sequence at %p", seq);
    seq->frames = NULL;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        void*, header, inMemFileReadStruct(file, WAX_SEQUENCE_FORMAT),
        {waxCloseSequence(seq);}
    )
    size_t length = getBlockLenghtFromFormat(WAX_SEQUENCE_FORMAT);
    memcpy(seq, header, length);
    memoryRelease(header);

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        ListOf(WAXFrame*)*, frames, waxReadFrames(file),
        {waxCloseSequence(seq);}
    );
    seq->frames = frames;

    return optionalOf(seq);
}

static OptionalOf(ListOf(WAXFrame*)*)* waxReadFrames(InMemoryFile* file){
    ListOf(WAXFrame*)* frames = listCreate(0);
    memoryTag(frames, "Frames");
    logTrace("Allocating Frame List at %p", frames);


    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        uint32_t*, frameOffsets, inMemFileReadStruct(file, UINT32_X_32),
        {waxCloseFrames(frames);}
    );

    logTrace("frame_offsets:");
    for(uint32_t i = 0; i < 32; i++) {
        uint32_t frameOffset = frameOffsets[i];
        if(!frameOffset) break;

        logTrace("- %d", frameOffset);
        memFileSeek(file, frameOffset, SEEK_SET);
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            WAXFrame*, frame, waxReadFrame(file),
            {waxCloseFrames(frames);}
        );
        listAppend(frames, frame);
    }

    memoryRelease(frameOffsets);

    return optionalOf(frames);
}

static OptionalOf(WAXFrame*)* waxReadFrame(InMemoryFile* file){
    WAXFrame* frame = memoryAllocateWithTag(sizeof(WAXFrame), "WAXFrame");
    logTrace("Allocating Frame at %p", frame);
    frame->cell = NULL;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        void*, header, inMemFileReadStruct(file, WAX_FRAME_FORMAT),
        {waxCloseFrame(frame);}
    )
    size_t length = getBlockLenghtFromFormat(WAX_FRAME_FORMAT);
    memcpy(frame, header, length);
    memoryRelease(header);

    memFileSeek(file, frame->cellOffset, SEEK_SET);
    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        WAXCell*, cell, waxReadCell(file),
        {waxCloseFrame(frame);}
    );
    frame->cell = cell;

    return optionalOf(frame);
}

static OptionalOf(WAXCell*)* waxReadCell(InMemoryFile* file) {
    WAXCell* cell = memoryAllocateWithTag(sizeof(WAXCell), "WAXCell");
    logTrace("Allocating Cell at %p", cell);
    cell->data = NULL;

    size_t cellPos = inMemFileCurrentPosition(file);

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        void*, header, inMemFileReadStruct(file, WAX_CELL_FORMAT),
        {waxCloseCell(cell);}
    )
    size_t length = getBlockLenghtFromFormat(WAX_CELL_FORMAT);
    memcpy(cell, header, length);
    memoryRelease(header);

    if(cell->compressed) {
        RLEConfig config = {
            .type = cell->compressed == 1 ? RLE0 : RLE1,
            .fileOffset = cellPos + cell->offset,
            .offsetCount = cell->width,
            .sequenceSize = cell->height
        };

        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint8_t*, data, rleReadFile(file, &config),
            {waxCloseCell(cell);}
        );
        cell->data = data;
    } else {
        uint32_t actualDataSize = cell->width * cell->height;
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint8_t*, data, inMemFileRead(file, actualDataSize),
            {waxCloseCell(cell);}
        );
        cell->data = data;
    }

    return optionalOf(cell);
}

void waxClose(WAXFile* wax) {
    if(!wax) return;
    logTrace("Releasing WAXFile");
    if(wax->header) memoryRelease(wax->header);
    if(wax->waxList) waxCloseWaxList(wax->waxList);

    memoryRelease(wax);
}

static void waxCloseWaxList(ListOf(WAX*)* waxList) {
    logTrace("Releasing ListOf(WAX)");
    forEachListItem(WAX*, wax, waxList, {
        waxCloseWax(wax);
    });

    listDelete(waxList);
}

static void waxCloseWax(WAX* wax) {
    logTrace("Releasing WAX");
    if(wax->sequences) waxCloseSequences(wax->sequences);

    memoryRelease(wax);
}

static void waxCloseSequences(ListOf(WAXSequence*)* sequences){
    logTrace("Releasing ListOf(WAXSequence)");
    forEachListItem(WAXSequence*, sequence, sequences, {
        waxCloseSequence(sequence);
    });

    listDelete(sequences);
}

static void waxCloseSequence(WAXSequence* sequence){
    logTrace("Releasing WAXSequence");
    if(sequence->frames) waxCloseFrames(sequence->frames);

    memoryRelease(sequence);
}

static void waxCloseFrames(ListOf(WAXFrame*)* frames){
    logTrace("Releasing ListOf(WAXFrame)");
    forEachListItem(WAXFrame*, frame, frames, {
        waxCloseFrame(frame);
    });

    listDelete(frames);
}

static void waxCloseFrame(WAXFrame* frame){
    logTrace("Releasing WAXFrame");
    if(frame->cell) waxCloseCell(frame->cell);

    memoryRelease(frame);
}

static void waxCloseCell(WAXCell* cell){
    logTrace("Releasing WAXCell");
    if(cell->data) memoryRelease(cell->data);
    memoryRelease(cell);
}

static void waxPrint(WAXFile* waxFile){
    logDebug("wax_file:");
    if(waxFile->header) {
        logDebug("  wax_header:");
        waxPrintHeader(waxFile->header);
    }
    if(waxFile->waxList) {
        logDebug("  waxes:");
        forEachListItem(WAX*, wax, waxFile->waxList, {
        waxPrintWAX(wax);
        });
    }
}

static void waxPrintHeader(WAXHeader* header){
    logDebug("    version: %d", header->version);
    logDebug("    sequences: %d", header->seqs);
    logDebug("    frames: %d", header->frames);
    logDebug("    cells: %d", header->cells);
    logDebug("    x_scale: %d", header->scaleX);
    logDebug("    y_scale: %d", header->scaleY);
    logDebug("    extra_light: %d", header->extraLight);
}

static void waxPrintWAX(WAX* wax) {
    logDebug("  - width: %d", wax->width);
    logDebug("    height: %d", wax->height);
    logDebug("    framerate: %d", wax->framerate);
    logDebug("    frames: %d", wax->frames);
    if(wax->sequences) {
        logDebug("    sequences:");
        forEachListItem(WAXSequence*, seq, wax->sequences, {
        waxPrintSeq(seq);
        })
    }
}

static void waxPrintSeq(WAXSequence* seq) {
    if(seq->frames) {
        logDebug("      frames:");
        forEachListItem(WAXFrame*, frame, seq->frames, {
           waxPrintFrame(frame);
        })
    }
}

static void waxPrintFrame(WAXFrame* frame) {
    logDebug("      - insert_x: %d", frame->insertX);
    logDebug("        insert_y: %d", frame->insertX);
    logDebug("        flip: %d", frame->flip);
    logDebug("        width: %d", frame->width);
    logDebug("        height: %d", frame->height);
    if(frame->cell) {
        logDebug("        cell:");
        waxPrintCell(frame->cell);
    }
}

static void waxPrintCell(WAXCell* cell) {
    logDebug("          width: %d", cell->width);
    logDebug("          height: %d", cell->height);
    logDebug("          compressed: %d", cell->compressed);
    logDebug("          dataSize: %d", cell->dataSize);
    logDebug("          offset: %d", cell->offset);
}
