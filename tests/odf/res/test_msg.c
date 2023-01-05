#include "test_fixtures.h"
#include "assertions/memory.h"
#include "assertions/optional.h"

#include "odf/res/msg.h"
#include "odf/res/types/msg_def.h"
#include "odf/sys/optional.h"
#include "odf/sys/stringbuffer.h"
#include "odf/sys/inmemoryfile.h"

StringBuffer* sb;

void setUp() {
    sb = stringBufferCreate();
}

void tearDown(){
    stringBufferDelete(sb);

    assertAllMemoryReleased();
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

void testNonMsgFile() {
    testCase("testNonMsgFile");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalIsEmpty(optMsg);
}

void testMinimalMsgFile() {
    testCase("testMinimalMsgFile");

    stringBufferAppend(sb, "MSG 1.0");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalNotEmpty(optMsg);
    MsgFile* msgFile = optionalGet(optMsg);
    assertEquali(1, msgFile->majorVersion);
    assertEquali(0, msgFile->minorVersion);

    msgCloseFile(msgFile);
}

void testDifferentVersionMsgFile() {
    testCase("testDifferentVersionMsgFile");

    stringBufferAppend(sb, "MSG 2.0");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalNotEmpty(optMsg);
    MsgFile* msgFile = optionalGet(optMsg);
    assertEquali(2, msgFile->majorVersion);
    assertEquali(0, msgFile->minorVersion);

    msgCloseFile(msgFile);
}

void testEmptyFirstLineMsgFile() {
    testCase("testEmptyFirstLineMsgFile");

    stringBufferAppend(sb, "\nMSG 2.0");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalNotEmpty(optMsg);
    MsgFile* msgFile = optionalGet(optMsg);
    assertEquali(2, msgFile->majorVersion);
    assertEquali(0, msgFile->minorVersion);

    msgCloseFile(msgFile);
}

void testCommentedOutFirstLineMsgFile() {
    testCase("testCommentedOutFirstLineMsgFile");

    stringBufferAppend(sb, "#MSG 1.0\nMSG 2.0");

    OptionalPtr* optMsg = buildMsg();

    assertOptionalNotEmpty(optMsg);
    MsgFile* msgFile = optionalGet(optMsg);
    assertEquali(2, msgFile->majorVersion);
    assertEquali(0, msgFile->minorVersion);

    msgCloseFile(msgFile);
}


int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testNonMsgFile,
        &testMinimalMsgFile,
        &testDifferentVersionMsgFile,
        &testEmptyFirstLineMsgFile,
        &testCommentedOutFirstLineMsgFile
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/res/msg.c";
    fixture.beforeEach = &setUp;
    fixture.afterEach = &tearDown;
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

