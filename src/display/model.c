#include <stdlib.h>
#include <GL/glew.h>
#include "display/model.h"

struct Model {
    uint32 VAO;
    uint32 VBO;
    uint32 EBO;
    uint32 indices;
};

Model* modelCreate(float* vertices, uint32 vsize,  uint32* indices, uint32 isize) {

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vsize, vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (0*sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (6*sizeof(float)));
    glEnableVertexAttribArray(1);

    Model* model = malloc(sizeof(Model));
    model->VAO = VAO;
    model->VBO = VBO;
    model->EBO = EBO;
    model->indices = isize;
    return model;
}

void modelDraw(Model* model) {
    glBindVertexArray(model->VAO);
    glDrawElements(GL_TRIANGLES, model->indices, GL_UNSIGNED_INT, 0);
}

void modelDelete(Model* model){
    glDeleteVertexArrays(1, &(model->VAO));
    glDeleteBuffers(1, &(model->VBO));
    glDeleteBuffers(1, &(model->EBO));
    free(model);
}

