/*******************************************************************
 *
 * File....: types.h
 * Function: Definition of basic types and global macros.
 * Author..: Thomas Schatton
 * Date....: 11/02/2010
 *
 ********************************************************************/

#ifndef TYPES_H_
#define TYPES_H_

#include <stdlib.h>
#include <stdio.h>
#include "memoryUsage.h"

static int returnCode; /* required for TRY(...) */

/* CALLOC: Allocate an array of _type with _num elements using _ptr.
 *         Returns EXIT_FAILURE if allocation fails. 
 */
#define CALLOC(_ptr, _num, _type)                                 \
	( _ptr = (_type *)calloc((_num), sizeof(_type))) == NULL        \
    ? EXIT_FAILURE                                                \
    : EXIT_SUCCESS		



/* ECALLOC: Allocate an array of _type with _num elements using
 *          _ptr. Returns _errorNr if allocation fails.
 */
#define ECALLOC(_ptr, _num, _type, _errorNr)                      \
	( _ptr = (_type *)calloc((_num), sizeof(_type))) == NULL        \
    ? _errorNr : EXIT_SUCCESS				                                        

/* TRY: Execute _function and return the error code if an error
 *      occurs (throw the error code to the parent function).
 */
#define TRY(_function)                                            \
  if ((returnCode = _function) > 0) return returnCode;

/* FREEPTR: Free a pointer. */
#define FREEPTR(_ptr)														                  \
{												                              						\
	free(_ptr);															                        \
	_ptr = NULL;														                        \
}

/* unsigned data types */
#ifndef BYTE 
	typedef unsigned char  BYTE;   // 1 Byte (x86)
#endif
typedef unsigned short USHORT; // 2 Byte (x86)
typedef unsigned int   UINT;   // 4 Byte (x86)
typedef unsigned long  ULONG;  // 4 Byte (x86)

/* boolean values */
#define BOOL  BYTE
#define FALSE 0
#ifndef TRUE
	#define TRUE  1u
#endif
#endif