/*****************************************************
* Datei: tstypes.h
* Autor: Thomas Schmiedel
* Datum: 02.11.2010
* Desc.: Definiert häufig verwendete Types und Macros
* Vers.: 1.0
* 19.05.2014 Strutz: CALLOC() included
******************************************************/

#ifndef _TS_TYPES_H_
#define _TS_TYPES_H_

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "memoryUsage.h"

extern unsigned int ERRCODE; /* in Form1.cpp */

/*** Speicher belegen und Rückgabe prüfen ***/
#define ALLOC(_ptr, _num, _type)										\
{																		\
	_ptr = (_type *)malloc((_num) * sizeof(_type));						\
	if (_ptr == NULL)													\
	{																	\
		fprintf(stderr, "malloc failed for %s %s\n", #_num, #_type);	\
		ERRCODE = MALLOC_FAILED;										\
	}																	\
}
/*** Speicher belegen und Rückgabe prüfen ***/
#ifndef CALLOC_
#define CALLOC_(_ptr, _num, _type)								\
{																								\
	_ptr = (_type *)calloc((_num), sizeof(_type));\
	if (_ptr == NULL)															\
	{																							\
		fprintf(stderr, "calloc failed for %s %s\n", #_num, #_type);	\
		ERRCODE = MALLOC_FAILED;										\
	}																							\
}
#endif

/*** Speicher freigeben ***/
#ifndef FREE
#define FREE(_ptr)														\
{																					\
	if (_ptr != NULL) free(_ptr);						\
	_ptr = NULL;														\
}
#endif

/* Passende inline-Statements für UNIX/WIN32 */
#ifdef WIN32
#define MYINLINE __inline
#else
#define MYINLINE __inline__
#endif

/*** Typedefs ***/
typedef unsigned long ulong;
typedef unsigned int  uint;
typedef unsigned char byte;
typedef unsigned char cbool;
typedef unsigned short uword;

/*** Zur Verwendung mit bool ***/
#ifndef TRUE
	#define TRUE 1u
#endif
#define FALSE 0

/*** Error Codes ***/
extern  uint ERRCODE; /* Form1.cpp	*/
#define PPM_OPEN_FAIL 1u
#define PPM_DAMAGED   2u
#define PPM_INVALID   3u
#define MALLOC_FAILED 4u
#define PRED_CRASHED  5u
#define CTX_NOT_SET   6u

#define _GETMIN(x, y) ((x) < (y) ? (x) : (y))
#define _GETMAX(x, y) ((x) > (y) ? (x) : (y))

#endif

