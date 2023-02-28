#include "test_fixtures.h"
#include "system/test_memory.h"
#include "assertions/optional.h"

#include "odf/res/fnt.h"
#include "odf/res/types/fnt_def.h"
#include "odf/sys/stringbuffer.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/list.h"

StringBuffer* sb;
char* fileContent;
InMemoryFile* fntInMemoryFile;
FNTFile* fnt;


int setUp(void** state) {
    sb = stringBufferCreate();
    fileContent = NULL;
    fntInMemoryFile = NULL;
    fnt = NULL;

    return 0;
}

int tearDown(void** state){
    stringBufferDelete(sb);
    if(fileContent) memoryRelease(fileContent);
    if(fntInMemoryFile) inMemFileDelete(fntInMemoryFile);
    if(fnt) fntClose(fnt);

    assert_int_equal(0, memoryGetAllocations());

    return 0;
}

OptionalPtr* fntReadStringBuffer(StringBuffer* sb) {
    uint32_t lenght = stringBufferSize(sb);
    char* fileContent = stringBufferToString(sb);
    fntInMemoryFile = optionalGet(memFileCreate(fileContent, lenght));
    return fntRead(fntInMemoryFile);

}

void testFntRead(){
    // Header
    stringBufferAppendBytes(sb, "FNT\x15", 4);                     // magic
    stringBufferAppendBytes(sb, "\x02", 1);                        // height
    stringBufferAppendBytes(sb, "\x00", 1);                        // unknown
    stringBufferAppendBytes(sb, "\x05\x00", 2);                    // dataSize
    stringBufferAppendBytes(sb, "a", 1);                           // firstChar
    stringBufferAppendBytes(sb, "a", 1);                           // lastChar
    stringBufferAppendBytes(sb, "                          ", 22); // padding

    // FNTCharacter A
    stringBufferAppendBytes(sb, "\x02", 1);             // width
    stringBufferAppendBytes(sb, "\x00\x01\x01\x01", 4); // data

    OptionalPtr* optFile = fntReadStringBuffer(sb);
    assertOptionalNotEmpty(optFile);
    fnt = optionalGet(optFile);

    assert_memory_equal("FNT\x15", fnt->header->magic, 4);
    assert_int_equal(2, fnt->header->height);
    assert_int_equal(5, fnt->header->dataSize);
    assert_int_equal('a', fnt->header->firstChar);
    assert_int_equal('a', fnt->header->lastChar);

    assert_int_equal(1, listSize(fnt->chars));
    FNTChar* fntChar = optionalGet(listGet(fnt->chars, 0));
    assert_int_equal(2, fntChar->width);
    assert_memory_equal("\x00\x01\x01\x01", fntChar->data, 4);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testFntRead, setUp, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/res/fnt.c", tests, NULL, NULL);

    return ret;
}
