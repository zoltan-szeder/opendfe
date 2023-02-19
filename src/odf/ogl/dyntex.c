#include "odf/ogl/dyntex.h"

#include <stdint.h>
#include <stddef.h>

#include "odf/sys/memory.h"
#include "odf/sys/list.h"
#include "odf/sys/optional.h"

struct OGLDynamicTexture {
    ListOf(OglTexture*)* textures;
    size_t currentTexture;
};

OGLDynamicTexture* oglDtCreateTexture() {
    OGLDynamicTexture* dyntex = memoryAllocate(sizeof(OGLDynamicTexture));

    dyntex->textures = listCreate(0);
    dyntex->currentTexture = 0;

    return dyntex;
}

void oglDtAddTexture(OGLDynamicTexture* dyntex, DglTexture* tex) {
    listAppend(dyntex->textures, tex);
}

DglTexture* oglDtGetCurrentTexture(OGLDynamicTexture* dyntex) {
    return optionalGet(listGet(dyntex->textures, dyntex->currentTexture));
}

void oglDtSwitchTexture(OGLDynamicTexture* dyntex) {
    dyntex->currentTexture = dyntex->currentTexture + 1 % listSize(dyntex->textures);
}

void oglDtDeleteTextures(OGLDynamicTexture* dyntex) {
    forEachListItem(DglTexture*, tex, dyntex->textures, {
        dglTextureDelete(tex);
    });

    listDelete(dyntex->textures);
    memoryRelease(dyntex);
}
