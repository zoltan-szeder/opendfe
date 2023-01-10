#include "test_fixtures.h"
#include "assertions/memory.h"
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

void setUp() {
    sb = stringBufferCreate();
}

void tearDown(){
    stringBufferDelete(sb);
    assertAllMemoryReleased();

    if(!memoryIsEmpty()) {
        memoryDump(false);
        memoryReleaseAll();
    }
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
    assertEquali(index, msg->index);
    assertEquali(priority, msg->priority);
    assertEquals(value, msg->value, strlen(value));
}

void testNonMsgFile() {
    testCase("testNonMsgFile");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testMinimalMsgFile() {
    testCase("testMinimalMsgFile");

    stringBufferAppend(sb, "MSG 1.0");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testDifferentVersionMsgFile() {
    testCase("testDifferentVersionMsgFile");

    stringBufferAppend(sb, "MSG 2.0");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testEmptyFirstLineMsgFile() {
    testCase("testEmptyFirstLineMsgFile");

    stringBufferAppend(sb, "\nMSG 2.0");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testInvalidVersion() {
    testCase("testInvalidVersion");

    stringBufferAppend(sb, "MSG MISSING");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testInvalidMessageCount() {
    testCase("testInvalidMessageCount");

    stringBufferAppend(
        sb,
        "MSG 1.0\n"
        "MSGS INVALID"
    );

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testInvalidMessage() {
    testCase("testInvalidMessage");

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
    testCase("testFullMsgFile");

    stringBufferAppend(sb, exampleMsg);

    OptionalPtr* optMsg = buildMsg();

    assertOptionalNotEmpty(optMsg);
    MsgFile* msgFile = optionalGet(optMsg);
    assertEquali(2, msgFile->majorVersion);
    assertEquali(0, msgFile->minorVersion);
    assertEquali(5, listSize( msgFile->messages));
    assertMsg(optionalGet(msgGet(msgFile, 0)), 0, 0, "message 1");
    assertMsg(optionalGet(msgGet(msgFile, 1)), 1, 0, "message 2");
    assertMsg(optionalGet(msgGet(msgFile, 2)), 2, 1, "message 3");
    assertMsg(optionalGet(msgGet(msgFile, 3)), 3, 2, "message 4");
    assertMsg(optionalGet(msgGet(msgFile, 4)), 4, 3, "message 5");

    msgCloseFile(msgFile);
}


int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testNonMsgFile,
        &testMinimalMsgFile,
        &testDifferentVersionMsgFile,
        &testEmptyFirstLineMsgFile,
        &testFullMsgFile,
        &testInvalidVersion,
        &testInvalidMessageCount,
        &testInvalidMessage,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/res/msg.c";
    fixture.beforeEach = &setUp;
    fixture.afterEach = &tearDown;
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

