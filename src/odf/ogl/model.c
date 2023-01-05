#include <stdlib.h>
#include <GL/glew.h>
#include "odf/ogl/model.h"
#include "odf/sys/memory.h"

struct Model {
    uint32_t vertexArray;
    uint32_t arrayBuffer;
    uint32_t elementArrayBuffer;
    uint32_t indices;
    DglTexture* texture;
};

Model* dglModelCreate(float* vertices, uint32_t vsize,  uint32_t* indices, uint32_t isize) {

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, indices, GL_STATIC_DRAW);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vsize, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (0*sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*) (6*sizeof(float)));
    glEnableVertexAttribArray(2);

    Model* model = memoryAllocateWithTag(sizeof(Model), "odf/ogl/model/Model");
    model->vertexArray = VAO;
    model->arrayBuffer = VBO;
    model->elementArrayBuffer = EBO;
    model->indices = isize;
    return model;
}

void dglModelBindTexture(Model* model, DglTexture* texture) {
    model->texture = texture;
}

void dglModelDraw(Model* model) {
    glBindVertexArray(model->vertexArray);
    dglTextureUse(model->texture);
    glDrawElements(GL_TRIANGLES, model->indices, GL_UNSIGNED_INT, 0);
}

void dglModelDelete(Model* model){
    glDeleteVertexArrays(1, &(model->vertexArray));
    glDeleteBuffers(1, &(model->arrayBuffer));
    glDeleteBuffers(1, &(model->elementArrayBuffer));
    memoryRelease(model);
}

