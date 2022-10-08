#include "test_fixtures.h"
#include "assertions/memory.h"
#include "assertions/optional.h"

#include "odf/res/fme.h"
#include "odf/sys/stringbuffer.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"

StringBuffer* sb;
char* fileContent;
InMemoryFile* fmeInMemoryFile;
FMEFile* fme;

OptionalPtr* fmeReadStringBuffer(StringBuffer* sb);

void setUp() {
    sb = stringBufferCreate();
    fileContent = NULL;
    fmeInMemoryFile = NULL;
    fme = NULL;
}

void tearDown(){
    stringBufferDelete(sb);
    if(fileContent) memoryRelease(fileContent);
    if(fmeInMemoryFile) inMemFileDelete(fmeInMemoryFile);
    if(fme) fmeClose(fme);

    assertAllMemoryReleased();
}

void testInvalidFmeRead(){
    testCase("testInvalidFmeRead");
    stringBufferAppend(sb, "\x00");

    OptionalPtr* optFile = fmeReadStringBuffer(sb);
    assertOptionalIsEmpty((void*) optFile);
}

void testMissingSubHeaderFmeRead(){
    testCase("testMissingSubHeaderFmeRead");

    // Header (0x00)
    stringBufferAppendBytes(sb, "\x01\x00\x00\x00", 4); // insertX
    stringBufferAppendBytes(sb, "\x02\x00\x00\x00", 4); // insertY
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // flip
    stringBufferAppendBytes(sb, "\x20\x00\x00\x00", 4); // subHeaderPointer
    stringBufferAppendBytes(sb, "\x03\x00\x00\x00", 4); // unitWidth
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4); // unitHeight
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00\x00\x00\x00\x00", 8); // padding

    OptionalPtr* optFile = fmeReadStringBuffer(sb);
    assertOptionalIsEmpty((void*) optFile);
}

void testFmeRead(){
    testCase("testFmeRead");

    // Header (0x00)
    stringBufferAppendBytes(sb, "\x01\x00\x00\x00", 4); // insertX
    stringBufferAppendBytes(sb, "\x02\x00\x00\x00", 4); // insertY
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // flip
    stringBufferAppendBytes(sb, "\x20\x00\x00\x00", 4); // subHeaderPointer
    stringBufferAppendBytes(sb, "\x03\x00\x00\x00", 4); // unitWidth
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4); // unitHeight
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00\x00\x00\x00\x00", 8); // padding

    // SubHeader (0x20)
    stringBufferAppendBytes(sb, "\x08\x00\x00\x00", 4); // SizeX
    stringBufferAppendBytes(sb, "\x08\x00\x00\x00", 4); // SizeY
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // Compressed
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // DataSize
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // ColOffs
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // pad1

    OptionalPtr* optFile = fmeReadStringBuffer(sb);
    assertOptionalNotEmpty(optFile);
    fme = optionalGet(optFile);

    assertEquali(1, fme->header->insertX);
    assertEquali(2, fme->header->insertY);
    assertEquali(FME_FLIP_NONE, fme->header->flip);
    assertEquali(32, fme->header->subHeaderPtr);
    assertEquali(3, fme->header->unitWidth);
    assertEquali(4, fme->header->unitHeight);

    assertEquali(8, fme->subHeader->sizeX);
    assertEquali(8, fme->subHeader->sizeY);
    assertEquali(FME_COMPRESS_NONE, fme->subHeader->compressed);
    assertEquali(0, fme->subHeader->dataSize);
    assertEquali(0, fme->subHeader->columnOffset);
}


OptionalPtr* fmeReadStringBuffer(StringBuffer* sb) {
    int size = stringBufferSize(sb);
    fileContent = stringBufferToString(sb);

    OptionalPtr* optInMemFile = memFileCreate(fileContent, size);
    fmeInMemoryFile = optionalGet(optInMemFile);

    return fmeOpenInMemoryFile(fmeInMemoryFile);
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testInvalidFmeRead,
        &testMissingSubHeaderFmeRead,
        &testFmeRead,
    };

    TestFixture fixture = createFixture();

    fixture.name = "drivers/fme.c";
    fixture.beforeEach = &setUp;
    fixture.afterEach = &tearDown;
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}
