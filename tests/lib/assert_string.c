#include "assertions/string.h"
#include "test_fixtures.h"


void assertNullTerminatedString(char* str, int length) {
    if(length > 10) {
        assertTrueMsg(str[length] == 0, error("\"%-7s...\" is not terminated with null character", str));
    } else {
        assertTrueMsg(str[length] == 0, error("\"%-10s\" is not terminated with null character", str));
    }
}
