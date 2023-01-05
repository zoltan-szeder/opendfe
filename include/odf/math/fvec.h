#ifndef MATH_FVEC_H
#define MATH_FVEC_H

#include <stdint.h>

typedef union {
    float data[4];
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    struct {
        float r;
        float g;
        float b;
        float a;
    };
    struct {
        float s;
        float t;
        float p;
        float q;
    };
} fvec4;

typedef union {
    float data[3];
    struct {
        float x;
        float y;
        float z;
    };
    struct {
        float r;
        float g;
        float b;
    };
    struct {
        float s;
        float t;
        float p;
    };
} fvec3;

typedef union {
    float data[2];
    struct {
        float x;
        float y;
    };
    struct {
        float r;
        float g;
    };
    struct {
        float s;
        float t;
    };
} fvec2;

#endif
