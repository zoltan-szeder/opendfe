#include "odf/sys/types.h"
#include "string.h"
#include "stdlib.h"
#include "odf/sys/strings.h"


bool inverseEndianness = false;


/*
 * Little endian:
 *
 *   0   1   2   3    Memory address
 * +---+---+---+---+
 * | 1 | 0 | 0 | 0 |  32bit integer of 1
 * +---+---+---+---+
 *   ^
 *   |
 * +---+
 * | 1 |  Test pointer
 * +---+
 *
 * Big endian:
 *
 *   0   1   2   3    Memory address
 * +---+---+---+---+
 * | 0 | 0 | 0 | 1 |  32bit integer of 1
 * +---+---+---+---+
 *   ^
 *   |
 * +---+
 * | 0 |  Test pointer
 * +---+
 */

bool isActuallyLittleEndian(){
    uint32 test_int = 1;
    uint8* test_char_arr = (uint8*) &test_int;

    return *test_char_arr;
}

bool isActuallyBigEndian(){
    return !isActuallyLittleEndian();
}

bool isLittleEndian(){
    if(inverseEndianness) {
        return !isActuallyLittleEndian();
    }
    return isActuallyLittleEndian();
}

bool isBigEndian(){
    if(inverseEndianness) {
        return !isActuallyBigEndian();
    }
    return isActuallyBigEndian();
}

void setToOriginalEndianness(){
    inverseEndianness = false;
}

void setToLittleEndian(){
    inverseEndianness = !isActuallyLittleEndian();
}

void setToBigEndian() {
    inverseEndianness = !isActuallyBigEndian();
}


void reverseEndianness(void* bytes, int lenght){
    uint8* array = bytes;
    for(int i = 0; i < lenght/2; i++) {
        int j = lenght - 1 - i;
        uint8 temp = array[i];
        array[i] = (uint8) array[j];
        array[j] = temp;
    }
}


void* modifyEndiannessOfStruct(void* object, const char* format) {
    int cursor = 0;
    for(size_t start = 0; start < strlen(format); ){
        size_t end = findFirstFrom(format, '%', start+1);
        char modifier = format[start+1];

        int length = atoi(format + start + 2);

        if(modifier == 'b' && !isBigEndian()) {
            reverseEndianness(object + cursor, length);
        }
        if(modifier == 'l' && !isLittleEndian()) {
            reverseEndianness(object + cursor, length);
        }

        cursor += length;
        start = end;
    }

    return object;
}

