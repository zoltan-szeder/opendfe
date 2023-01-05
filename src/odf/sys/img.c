#include <stdlib.h>
#include "odf/sys/img.h"
#include "odf/sys/memory.h"

Image8Bit* img8bCreate2D(uint32_t width, uint32_t height, uint32_t channels) {
    uint32_t depth = 1;
    uint32_t size = width*height*depth*channels*sizeof(uint8_t);
    uint8_t* data = memoryAllocateWithTag(size, "odf/sys/img/Image8Bit/data");
    if(data == NULL) return NULL;

    Image8Bit* img = memoryAllocateWithTag(sizeof(Image8Bit), "odf/sys/img/Image8Bit");
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
