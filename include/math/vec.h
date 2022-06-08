#ifndef MATH_VEC_H
#define MATH_VEC_H

typedef union {
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
