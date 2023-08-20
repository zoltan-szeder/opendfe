#include "odf/res/3do.h"
#include "odf/res/types/3do_def.h"

#include "test_fixtures.h"
#include "system/test_memory.h"
#include "assertions/optional.h"

#include "odf/sys/log.h"
#include "odf/sys/memory.h"
#include "odf/sys/stringbuffer.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"


StringBuffer* sb = NULL;
InMemoryFile* file = NULL;
TDOFile* tdo = NULL;

int setUp(void** state){
    logSetLevel(WARN);

    sb = stringBufferCreate();

    return 0;
}

int tearDown(void** state){
    if(sb) stringBufferDelete(sb);
    if(file) inMemFileDelete(file);
    if(tdo) tdoCloseFile(tdo);

    if(memoryGetAllocations() != 0) {
        memoryDump(false);
        memoryReleaseAll();
        fail_msg("Found remaining memory");
    }
    
    return 0;
}

OptionalOf(TDOFile*)* readFile() {
    char* str = stringBufferToString(sb);
    file = optionalGet(memFileCreate(str, stringBufferSize(sb)));
    memoryRelease(str);
    return tdoOpenFile(file);
} 

void test3doReadEmptyFile() {
    OptionalPtr* optTdoFile = readFile();
    assertOptionalIsEmpty(optTdoFile);
}

void testReadMinimal3doFile() {
    stringBufferAppend(sb, "3DO 1.2");

    tdo = optionalGet(readFile());

    tdoCloseFile(tdo);
}


int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test3doReadEmptyFile, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testReadMinimal3doFile, setUp, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/res/3do.c", tests, NULL, NULL);

    return ret;
}
