#include "odf/sys/log.h"

#include <stdarg.h>

static FILE* LOG_STREAM = NULL;
static LogLevel LOG_LEVEL = INFO;


void logSetLevel(LogLevel level) {
    LOG_LEVEL = level;
}

void logSetStream(FILE* stream) {
    LOG_STREAM = stream;
}

void logPrint(LogLevel level, const char* format, ...) {
    if(level < LOG_LEVEL) return;

    if(!LOG_STREAM) LOG_STREAM = stderr;

    va_list args;
    va_start(args, format);

    vfprintf(LOG_STREAM, format, args);
    fprintf(LOG_STREAM, "\n");
    fflush(LOG_STREAM);

    va_end(args);
}
