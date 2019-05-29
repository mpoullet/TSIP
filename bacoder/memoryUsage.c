/*****************************************************
 * File..: memoryUsage.c
 * Desc..: overloading malloc() and calloc() and free()
 * Author: T. Strutz
 * Date..: 03.05.2016
 ******************************************************/

#include "memoryUsage.h"
#include <malloc.h>

static unsigned long long maxMem = 0;
static unsigned long long curMem = 0;

/*--------------------------------------------------------
* mallocUsage()
*-------------------------------------------------------*/
void* mallocUsage( int num)
{
	curMem += num;
	if (maxMem < curMem) 
		maxMem = curMem;
	return (void*)malloc( num);
}
/*--------------------------------------------------------
* callocUsage()
*-------------------------------------------------------*/
void* callocUsage( int num, int sizeofType)
{
	curMem += num * sizeofType;
	if (maxMem < curMem) maxMem = curMem;
  return (void*)calloc( num, sizeofType);
}

/*--------------------------------------------------------
* freeUsage()
*-------------------------------------------------------*/
void freeUsage( void *pointer)
{
  free( pointer);
	//curMem -= num;
}

