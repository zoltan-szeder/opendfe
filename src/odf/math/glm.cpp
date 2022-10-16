#include <odf/math/glm.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static glm::mat4 toGlmMat4(mat4);
static mat4 toMat4(glm::mat4 mat);

static glm::vec3 toGlmVec3(fvec3);

EXTERNC mat4 mat4Unit() {
    return toMat4(glm::mat4(1.0f));
}

EXTERNC mat4 mat4Translate(mat4 matrix, fvec3 translation) {
    return toMat4(
        glm::translate(
            toGlmMat4(matrix),
            toGlmVec3(translation)
        )
    );
}

EXTERNC mat4 mat4Scale(mat4 matrix, fvec3 scale) {
    return toMat4(
        glm::scale(
            toGlmMat4(matrix),
            toGlmVec3(scale)
        )
    );
}

EXTERNC mat4 mat4Rotate(mat4 matrix, float radians, fvec3 axis) {
    return toMat4(
        glm::rotate(
            toGlmMat4(matrix),
            glm::radians(radians),
            toGlmVec3(axis)
        )
    );
}


EXTERNC mat4 mat4Perspective(float fowDegrees, float aspectRatio, float near, float far) {
    return toMat4(
        glm::perspective(
            glm::radians(fowDegrees),
            aspectRatio,
            near,
            far
        )
    );
}


static glm::mat4 toGlmMat4(mat4 mat) {
    return glm::mat4(
        mat.cols[0].x, mat.cols[0].y, mat.cols[0].z, mat.cols[0].w,
        mat.cols[1].x, mat.cols[1].y, mat.cols[1].z, mat.cols[1].w,
        mat.cols[2].x, mat.cols[2].y, mat.cols[2].z, mat.cols[2].w,
        mat.cols[3].x, mat.cols[3].y, mat.cols[3].z, mat.cols[3].w
    );
}

static mat4 toMat4(glm::mat4 glmMat) {
    mat4 mat;
    float* matPtr = glm::value_ptr(glmMat);

    for(int i = 0; i < 16; i++) {
        mat.values[i] = matPtr[i];
    }

    return mat;
}

static glm::vec3 toGlmVec3(fvec3 vec) {
    return glm::vec3(vec.x, vec.y, vec.z);
}
