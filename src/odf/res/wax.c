#include "odf/res/wax.h"
#include "odf/res/types/wax_def.h"

#include <string.h>

#include "odf/sys/list.h"
#include "odf/sys/memory.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/strings.h"
#include "odf/sys/optional.h"
#include "odf/sys/log.h"

static OptionalOf(ListOf(WAX*)*)* waxReadWaxList(InMemoryFile* file);
static OptionalOf(WAX*)* waxReadWax(InMemoryFile* file);
static OptionalOf(ListOf(WAXSequence*)*)* waxReadSequences(InMemoryFile* file);
static OptionalOf(WAXSequence*)* waxReadSequence(InMemoryFile* file);

static void waxCloseWaxList(ListOf(WAX*)* waxList);
static void waxCloseWax(WAX* wax);
static void waxCloseSequences(ListOf(WAXSequence*)* sequences);
static void waxCloseSequence(WAXSequence* sequence);

static void waxPrint(WAXFile* waxFile);
static void waxPrintHeader(WAXHeader* header);
static void waxPrintWAX(WAX* wax);
static void waxPrintSeq(WAXSequence* seq);
static void waxPrintFrame(WAXFrame* frame);

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

    logDebug("wax_offsets:");
    for(uint32_t i = 0; i < 32; i++) {
        uint32_t waxOffset = waxOffsets[i];
        if(!waxOffset) break;

        logTrace("- %d", waxOffset);
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

    logDebug("seq_offsets:");
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

    return optionalOf(seq);
}

static void waxCloseWaxList(ListOf(WAX*)* waxList);
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
    memoryRelease(sequence);
}

static void waxPrint(WAXFile* waxFile){
    logDebug("wax_file:");
    logDebug("  wax_header:");
    waxPrintHeader(waxFile->header);
    logDebug("  waxes:");
    forEachListItem(WAX*, wax, waxFile->waxList, {
        waxPrintWAX(wax);
    });
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
    logDebug("    sequences:");
    forEachListItem(WAXSequence*, seq, wax->sequences, {
        waxPrintSeq(seq);
    })
}

static void waxPrintSeq(WAXSequence* seq) {
    logDebug("    - frames:");
    // forEachListItem(WAXFrame*, frame, seq->frames, {
    //	   waxPrintFrame(frame);
    // })
}

static void waxPrintFrame(WAXFrame* frame) {

}
