#ifndef MATH_VEC_H
#define MATH_VEC_H

#include <stdint.h>

typedef union {
    uint8_t data[4];
    struct {
        uint8_t x;
        uint8_t y;
        uint8_t z;
        uint8_t w;
    };
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    struct {
        uint8_t s;
        uint8_t t;
        uint8_t p;
        uint8_t q;
    };
} ucvec4;

typedef union {
    uint8_t data[3];
    struct {
        uint8_t x;
        uint8_t y;
        uint8_t z;
    };
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    struct {
        uint8_t s;
        uint8_t t;
        uint8_t p;
    };
} ucvec3;

typedef union {
    uint8_t data[2];
    struct {
        uint8_t x;
        uint8_t y;
    };
    struct {
        uint8_t r;
        uint8_t g;
    };
    struct {
        uint8_t s;
        uint8_t t;
    };
} ucvec2;

#endif
