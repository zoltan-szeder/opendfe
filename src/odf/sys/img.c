#include <stdlib.h>
#include "odf/sys/img.h"
#include "odf/sys/memory.h"

Image8Bit* img8bCreate2D(uint32 width, uint32 height, uint32 channels) {
    uint32 depth = 1;
    uint32 size = width*height*depth*channels*sizeof(uint8);
    uint8* data = memoryAllocate(size);
    if(data == NULL) return NULL;

    Image8Bit* img = memoryAllocate(sizeof(Image8Bit));
    if(img == NULL) return NULL;

    img->width = width;
    img->height = height;
    img->depth = 1;
    img->channels = channels;
    img->size = size;
    img->data = data;
    return img;

}

void img8bDelete(Image8Bit* img) {
    memoryRelease(img->data);
    memoryRelease(img);
}
