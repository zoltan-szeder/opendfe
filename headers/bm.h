#ifndef BM_H
#define BM_H

typedef struct BMFile BMFile;

BMFile* bmOpenFile(char*);
void bmCloseFile(BMFile*);

void bmPrintFile(BMFile*);

#endif
