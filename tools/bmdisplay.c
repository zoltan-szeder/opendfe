#include <stdio.h>
#include <stdlib.h>

#include "dgl/main.h"
#include "dgl/model.h"
#include "dgl/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


int main(int argc, char** argv) {
    Display* display = dglCreateDisplay();

    if(argc < 2) return 1;

    int width, height, channels;
    unsigned char *data = stbi_load(argv[1], &width, &height, &channels, 0);

    Image8Bit* image = img8bCreate2D(width, height, channels);
    image->data = data;

    DglTexture* texture = dglTextureCreate(image);
    img8bDelete(image);

    float vertices[]= {
    //     x      y     z       r     g     b       s     t
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    GameState gamestate;
    gamestate.model = dglModelCreate(vertices, sizeof(vertices), indices, sizeof(indices));
    dglModelBindTexture(gamestate.model, texture);

    GameControl gameControl;
    gameControl.shouldExit = false;
    while(!gameControl.shouldExit)
    {
        dglReadInputs(display, &gameControl);

        dglDraw(display, &gamestate);
    }

    dglDestroyDisplay(display);
    dglModelDelete(gamestate.model);
    dglTextureDelete(texture);

    return 0;
}
