#ifndef GL_MIPMAP_H
#define GL_MIPMAP_H

typedef struct Mipmap {
    int width;
    int height;
    int channels;

    unsigned char* data;
} Mipmap;

Mipmap* openMipmap(char*);
void closeMipmap(Mipmap*);

#endif
