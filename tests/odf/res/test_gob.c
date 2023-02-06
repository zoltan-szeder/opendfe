#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_fixtures.h"
#include "system/test_memory.h"

#include "odf/res/gob.h"
#include "odf/sys/list.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"

void tearDown(){
    assert_true(memoryGetAllocations() > 0);
}

void testOpenMissingFile(){
    GobArchive* archive = optionalGet(gobOpenArchive("tests/resources/test.gob"));
    List* files = gobListFiles(archive);
    GobFile* file = optionalGet(listGet(files, 0));

    assert(archive != NULL);
    assert(gobCountFiles(archive) == 1);
    assert(strcmp("HELLO.TXT", gobGetFileName(file)) == 0);
    
    gobCloseArchive(archive);
}

void testReadFile() {
    GobArchive* archive = optionalGet(gobOpenArchive("tests/resources/test.gob"));
    GobFile* helloFile = optionalGet(gobGetFile(archive, "HELLO.TXT"));
    InMemoryFile* hello = optionalGet(gobReadFile(helloFile));

    OptionalPtr* optContent = inMemFileRead(hello, inMemFileSize(hello));
    if(optionalIsEmpty(optContent)) {
        fail();
        return;
    }

    char* content = optionalGet(optContent);

    assert(gobCountFiles(archive) == 1);
    assert_memory_equal("Hello World", content, 12);

    memoryRelease(content);
    inMemFileDelete(hello);
    gobCloseArchive(archive);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testOpenMissingFile, NULL, NULL),
        cmocka_unit_test_setup_teardown(testReadFile, NULL, NULL),
    };

    int ret = cmocka_run_group_tests_name("odf/res/gob.c", tests, NULL, NULL);

    return ret;
}
