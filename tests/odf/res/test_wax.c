
#include "test_fixtures.h"
#include "system/test_memory.h"
#include "assertions/optional.h"

#include "odf/res/wax.h"
#include "odf/res/types/wax_def.h"
#include "odf/sys/stringbuffer.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/list.h"

StringBuffer* sb;
char* fileContent;
InMemoryFile* waxInMemoryFile;
WAXFile* wax;


int setUp(void** state) {
    sb = stringBufferCreate();
    fileContent = NULL;
    waxInMemoryFile = NULL;
    wax = NULL;

    return 0;
}

int tearDown(void** state){
    stringBufferDelete(sb);
    if(fileContent) memoryRelease(fileContent);
    if(waxInMemoryFile) inMemFileDelete(waxInMemoryFile);
    if(wax) waxClose(wax);

    assert_int_equal(0, memoryGetAllocations());

    return 0;
}

OptionalPtr* waxReadStringBuffer(StringBuffer* sb) {
    uint32_t lenght = stringBufferSize(sb);
    char* fileContent = stringBufferToString(sb);
    waxInMemoryFile = optionalGet(memFileCreate(fileContent, lenght));
    return waxReadInMemoryFile(waxInMemoryFile);

}

void testWaxReadEmptyFile(){
    OptionalPtr* optFile = waxReadStringBuffer(sb);
    assertOptionalIsEmpty(optFile);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testWaxReadEmptyFile, setUp, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/res/wax.c", tests, NULL, NULL);

    return ret;
}
