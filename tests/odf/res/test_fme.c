#include "test_fixtures.h"
#include "system/test_memory.h"
#include "assertions/optional.h"

#include "odf/res/fme.h"
#include "odf/res/types/fme_def.h"
#include "odf/sys/stringbuffer.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/log.h"

StringBuffer* sb;
char* fileContent;
InMemoryFile* fmeInMemoryFile;
FMEFile* fme;

OptionalPtr* fmeReadStringBuffer(StringBuffer* sb);

int setUp(void** state) {
    logSetLevel(TRACE);
    sb = stringBufferCreate();
    fileContent = NULL;
    fmeInMemoryFile = NULL;
    fme = NULL;

    return 0;
}

int tearDown(void** state){
    stringBufferDelete(sb);
    if(fileContent) memoryRelease(fileContent);
    if(fmeInMemoryFile) inMemFileDelete(fmeInMemoryFile);
    if(fme) fmeClose(fme);

    assert_int_equal(0, memoryGetAllocations());

    return 0;
}

void testInvalidFmeRead(){
    stringBufferAppend(sb, "\x00");

    OptionalPtr* optFile = fmeReadStringBuffer(sb);
    assertOptionalIsEmpty((void*) optFile);
}

void testMissingSubHeaderFmeRead(){
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
    // Header (0x00)
    stringBufferAppendBytes(sb, "\x01\x00\x00\x00", 4); // insertX
    stringBufferAppendBytes(sb, "\x02\x00\x00\x00", 4); // insertY
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // flip
    stringBufferAppendBytes(sb, "\x20\x00\x00\x00", 4); // subHeaderPointer
    stringBufferAppendBytes(sb, "\x03\x00\x00\x00", 4); // unitWidth
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4); // unitHeight
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00\x00\x00\x00\x00", 8); // padding

    // SubHeader (0x20)
    stringBufferAppendBytes(sb, "\x02\x00\x00\x00", 4); // SizeX
    stringBufferAppendBytes(sb, "\x02\x00\x00\x00", 4); // SizeY
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // Compressed
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // DataSize
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // ColOffs
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // pad1



    // Data
    stringBufferAppendBytes(sb, "\x01\x02\x03\x04", 4); // pad1

    OptionalPtr* optFile = fmeReadStringBuffer(sb);
    assertOptionalNotEmpty(optFile);
    fme = optionalGet(optFile);

    assert_int_equal(1, fme->header->insertX);
    assert_int_equal(2, fme->header->insertY);
    assert_int_equal(FME_FLIP_NONE, fme->header->flip);
    assert_int_equal(32, fme->header->subHeaderPtr);
    assert_int_equal(3, fme->header->unitWidth);
    assert_int_equal(4, fme->header->unitHeight);

    assert_int_equal(2, fme->subHeader->sizeX);
    assert_int_equal(2, fme->subHeader->sizeY);
    assert_int_equal(FME_COMPRESS_NONE, fme->subHeader->compressed);
    assert_int_equal(28, fme->subHeader->dataSize);
    assert_int_equal(0, fme->subHeader->columnOffset);

    assert_non_null(fme->data);
    assert_memory_equal("\x01\x02\x03\x04", fme->data, 4);
}


void testCompressedFmeRead(){
    // Header (0x00)
    stringBufferAppendBytes(sb, "\x01\x00\x00\x00", 4); // 0x00 insertX
    stringBufferAppendBytes(sb, "\x02\x00\x00\x00", 4); // 0x04 insertY
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x08 flip
    stringBufferAppendBytes(sb, "\x20\x00\x00\x00", 4); // 0x0c subHeaderPointer
    stringBufferAppendBytes(sb, "\x03\x00\x00\x00", 4); // 0x10 unitWidth
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4); // 0x14 unitHeight
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x18 padding
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x1c padding

    // SubHeader (0x20)
    stringBufferAppendBytes(sb, "\x03\x00\x00\x00", 4); // 0x20 SizeX
    stringBufferAppendBytes(sb, "\x02\x00\x00\x00", 4); // 0x24 SizeY
    stringBufferAppendBytes(sb, "\x01\x00\x00\x00", 4); // 0x28 Compressed
    stringBufferAppendBytes(sb, "\x1e\x00\x00\x00", 4); // 0x2c DataSize
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x30 ColOffs
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x34 pad1


    // ColumnOffsets
    stringBufferAppendBytes(sb, "\x24\x00\x00\x00", 4); // 0x38 offset 1
    stringBufferAppendBytes(sb, "\x25\x00\x00\x00", 4); // 0x3c offset 2
    stringBufferAppendBytes(sb, "\x28\x00\x00\x00", 4); // 0x40 offset 3

    // Data
    stringBufferAppendBytes(sb, "\x82\x02\x03\x04", 4); // 0x44 (0x24)
    stringBufferAppendBytes(sb, "\x02\x05\x06", 3);     // 0x48 (0x28)

    OptionalPtr* optFile = fmeReadStringBuffer(sb);
    assertOptionalNotEmpty(optFile);
    fme = optionalGet(optFile);

    assert_int_equal(1, fme->header->insertX);
    assert_int_equal(2, fme->header->insertY);
    assert_int_equal(FME_FLIP_NONE, fme->header->flip);
    assert_int_equal(32, fme->header->subHeaderPtr);
    assert_int_equal(3, fme->header->unitWidth);
    assert_int_equal(4, fme->header->unitHeight);

    assert_int_equal(3, fme->subHeader->sizeX);
    assert_int_equal(2, fme->subHeader->sizeY);
    assert_int_equal(FME_COMPRESS_RLE0, fme->subHeader->compressed);
    assert_int_equal(30, fme->subHeader->dataSize);
    assert_int_equal(0, fme->subHeader->columnOffset);

    assert_non_null(fme->data);
    assert_memory_equal("\x00\x00\x03\x04\x05\x06", fme->data, 6);
}


OptionalPtr* fmeReadStringBuffer(StringBuffer* sb) {
    int size = stringBufferSize(sb);
    fileContent = stringBufferToString(sb);

    OptionalPtr* optInMemFile = memFileCreate(fileContent, size);
    fmeInMemoryFile = optionalGet(optInMemFile);

    return fmeOpenInMemoryFile(fmeInMemoryFile);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testInvalidFmeRead, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testMissingSubHeaderFmeRead, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testFmeRead, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testCompressedFmeRead, setUp, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/res/fme.c", tests, NULL, NULL);

    return ret;
}
