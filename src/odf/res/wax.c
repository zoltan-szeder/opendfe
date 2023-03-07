#include "odf/res/wax.h"
#include "odf/res/types/wax_def.h"

OptionalOf(WAXFile*)* waxReadInMemoryFile(InMemoryFile* file) {
    return optionalEmpty("Not Implemented");
}

void waxClose(WAXFile* wax) {
    if(!wax) return;
}
