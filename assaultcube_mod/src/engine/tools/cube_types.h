#ifndef CUBETYPES_H
#define CUBETYPES_H
#include <stdio.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

#if !defined(WIN32)
typedef int BOOL;
typedef int INT;
typedef char* PCHAR;
#endif

#if defined(TRUE)
#undef TRUE;
#endif
#if defined(FALSE)
#undef FALSE;
#endif

#define TRUE 1
#define FALSE 0

#endif