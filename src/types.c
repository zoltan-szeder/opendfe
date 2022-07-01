#include "types.h"

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
bool isLittleEndian(){
    uint32 test_int = 1;
    uint8* test_char_arr = (uint8*) &test_int;

    return *test_char_arr;
}

bool isBigEndian(){
    return !isLittleEndian();
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

