#include "odf/sys/strings.h"

#include <string.h>
#include <stdlib.h>

#include "odf/sys/types.h"
#include "odf/sys/list.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"

long findFirstFrom(const char* str, char ch, size_t offset) {
    size_t size = strlen(str);

    for(size_t i = offset; i < size; i++) {
        if(str[i] == ch) {
            return offset;
        }
    }

    return size;
}


size_t getBlockLenghtFromFormat(const char* format) {
    size_t size = 0;

    for(size_t start = 0; start < strlen(format); ){
        size += atoi(format + start + 2);
        size_t end = findFirstFrom(format, '%', start+1);
        start = end;
    }

    return size;
}

static uint8_t lineBreakSize(char* c) {
    if(*c == '\r')
        // Windows-style line breaks
        if(*(c+1) == '\n')
            return 2;
        // Mac-style line breaks
        else
            return 1;
    // UNIX style line breaks
    else if (*c == '\n')
        return 1;
    else
        return 0;
}

static size_t findNextBreak(char* content) {
    for(size_t i = 0; content[i] != 0; i++)
        if(content[i] == '\r' || content[i] == '\n')
            return i;

    return -1;
}

static uint32_t countLines(char* content) {
    uint32_t lines = 1;

    for(size_t i = 0; content[i] != 0;) {
        char* cursor = content + i;

        size_t nextBreak  = findNextBreak(cursor);

        if(nextBreak == -1) {
            return lines;
        }

        lines++;
        i += nextBreak + lineBreakSize(cursor + nextBreak);
    }

    return lines;
}

List* strSplitByLineBreak(char* content) {
    uint32_t lineCount = countLines(content);
    List* lines = listCreate(lineCount);

    char* cursor = content;
    for(size_t i = 0; i < lineCount; i++) {
        int32_t lineLength = findNextBreak(cursor);

        if(lineLength < 0)
            lineLength = strlen(cursor);

        char* line = memoryAllocateWithTag(lineLength+1, "line");
        memcpy(line, cursor, lineLength);
        line[lineLength] = 0;

        listAppend(lines, line);
        uint8_t lineBreakLength = lineBreakSize(cursor + lineLength);
        cursor += lineLength + lineBreakLength;
    }

    return lines;
}

