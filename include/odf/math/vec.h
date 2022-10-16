#ifndef MATH_VEC_H
#define MATH_VEC_H

#include "odf/sys/types.h"

typedef union {
    uint8 data[4];
    struct {
        unsigned char x;
        unsigned char y;
        unsigned char z;
        unsigned char w;
    };
    struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };
    struct {
        unsigned char s;
        unsigned char t;
        unsigned char p;
        unsigned char q;
    };
} ucvec4;

typedef union {
    uint8 data[3];
    struct {
        unsigned char x;
        unsigned char y;
        unsigned char z;
    };
    struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };
    struct {
        unsigned char s;
        unsigned char t;
        unsigned char p;
    };
} ucvec3;

typedef union {
    uint8 data[2];
    struct {
        unsigned char x;
        unsigned char y;
    };
    struct {
        unsigned char r;
        unsigned char g;
    };
    struct {
        unsigned char s;
        unsigned char t;
    };
} ucvec2;

#endif
