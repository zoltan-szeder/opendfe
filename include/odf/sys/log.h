#ifndef ODF_SYS_LOG_H
#define ODF_SYS_LOG_H

#include <stdio.h>

#include "odf/sys/macros.h"

typedef enum {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
} LogLevel;

void logSetLevel(LogLevel level);
void logSetStream(FILE* stream);

void logPrint(LogLevel level, const char* format, ...);

// Helper macros
#define logPrintDetailed(level, ...) logPrint(level, __FILE__ ":L" LINE_NUMBER_AS_STRING " - " __VA_ARGS__)
#define logTrace(...) logPrintDetailed(TRACE, __VA_ARGS__)
#define logDebug(...) logPrintDetailed(DEBUG, __VA_ARGS__)
#define logInfo(...) logPrintDetailed(INFO, __VA_ARGS__)
#define logWarn(...) logPrintDetailed(WARN, __VA_ARGS__)
#define logError(...) logPrintDetailed(ERROR, __VA_ARGS__)
#define logFatal(...) logPrintDetailed(FATAL, __VA_ARGS__)

#endif
