#ifndef ODF_MATH_MAT_H
#define ODF_MATH_MAT_H

#include "odf/math/fvec.h"

typedef union {
    fvec4 cols[4];
    float values[4*4];
} mat4;

#endif
