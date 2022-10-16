#ifndef ODF_MATH_GLM_H
#define ODF_MATH_GLM_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#include "odf/math/fvec.h"
#include "odf/math/mat.h"

EXTERNC mat4 mat4Unit();
EXTERNC mat4 mat4Translate(mat4 matrix, fvec3 translation);
EXTERNC mat4 mat4Scale(mat4 matrix, fvec3 scale);
EXTERNC mat4 mat4Rotate(mat4 matrix, float radians, fvec3 axis);
EXTERNC mat4 mat4Perspective(float fowRadians, float aspectRatio, float near, float far);

#endif
