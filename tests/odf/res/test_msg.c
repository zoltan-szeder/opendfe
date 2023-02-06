#include "test_fixtures.h"
#include "system/test_memory.h"
#include "assertions/optional.h"

#include "odf/res/msg.h"
#include "odf/res/types/msg_def.h"

#include <string.h>

#include "odf/sys/optional.h"
#include "odf/sys/list.h"
#include "odf/sys/stringbuffer.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/log.h"

static const char* exampleMsg =
    "#MSG 1.0\n"
    "MSG 2.0\n"
    "\n"
    "MSGS 5\n"
    "\n"
    "# internal game messages\n"
    "# <INDEX> <PRIORITY>: \"<MESSAGE>\"\n"
    "0 0: \"message 1\"\n"
    "\t1 0: \"message 2\"\n"
    "    2\t1:\t\"message 3\"\n"
    "\t3\t2:\t\"message 4\" # This is a comment\n"
    "\t4\t3:\t\"message 5\" # This is a \"comment\"\n"
    "END";

StringBuffer* sb;

int setUp(void** state) {
    sb = stringBufferCreate();

    return 0;
}

int tearDown(void** state){
    stringBufferDelete(sb);
    assert_int_equal(0, memoryGetAllocations());

    if(!memoryIsEmpty()) {
        memoryDump(false);
        memoryReleaseAll();
    }

    return 0;
}

static OptionalPtr* buildMsg() {
    InMemoryFile* file = optionalGet(memFileCreate(
        stringBufferToString(sb),
        stringBufferSize(sb))
    );

    OptionalPtr* optMsg = msgOpenFile(file);

    inMemFileDelete(file);

    return optMsg;
}

static void assertMsg(Msg* msg, size_t index, size_t priority, char* value) {
    assert_int_equal(index, msg->index);
    assert_int_equal(priority, msg->priority);
    assert_memory_equal(value, msg->value, strlen(value));
}

void testNonMsgFile() {
    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testMinimalMsgFile() {
    stringBufferAppend(sb, "MSG 1.0");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testDifferentVersionMsgFile() {
    stringBufferAppend(sb, "MSG 2.0");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testEmptyFirstLineMsgFile() {
    stringBufferAppend(sb, "\nMSG 2.0");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testInvalidVersion() {
    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testInvalidMessageCount() {
    stringBufferAppend(
        sb,
        "MSG 1.0\n"
        "MSGS INVALID"
    );

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testInvalidMessage() {
    stringBufferAppend(
        sb,
        "MSG 1.0\n"
        "MSGS 1\n"
        "0: 1: \"MissingClosingQuotationMark\n"
        "END"
    );

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testFullMsgFile() {
    stringBufferAppend(sb, exampleMsg);

    OptionalPtr* optMsg = buildMsg();

    assertOptionalNotEmpty(optMsg);
    MsgFile* msgFile = optionalGet(optMsg);
    assert_int_equal(2, msgFile->majorVersion);
    assert_int_equal(0, msgFile->minorVersion);
    assert_int_equal(5, listSize( msgFile->messages));
    assertMsg(optionalGet(msgGet(msgFile, 0)), 0, 0, "message 1");
    assertMsg(optionalGet(msgGet(msgFile, 1)), 1, 0, "message 2");
    assertMsg(optionalGet(msgGet(msgFile, 2)), 2, 1, "message 3");
    assertMsg(optionalGet(msgGet(msgFile, 3)), 3, 2, "message 4");
    assertMsg(optionalGet(msgGet(msgFile, 4)), 4, 3, "message 5");

    msgCloseFile(msgFile);
}


int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testNonMsgFile, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testMinimalMsgFile, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testDifferentVersionMsgFile, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testEmptyFirstLineMsgFile, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testFullMsgFile, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testInvalidVersion, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testInvalidMessageCount, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testInvalidMessage, setUp, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/res/msg.c", tests, NULL, NULL);

    return ret;
}

