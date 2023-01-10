#ifndef ODF_SYS_MACROS_H
#define ODF_SYS_MACROS_H

#define AS_STRING(number) AS_STRING2(number)
#define AS_STRING2(number) #number
#define LINE_NUMBER_AS_STRING AS_STRING(__LINE__)


#endif
