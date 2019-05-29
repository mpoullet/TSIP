/*****************************************************
 * File..: memoryUsage.h
 * Desc..: overloading malloc() and calloc() and free()
 * Author: T. Strutz
 * Date..: 03.05.2016
 ******************************************************/

//#define calloc callocUsage
//#define malloc mallocUsage
//#define free freeUsage

void* mallocUsage( int num);
void* callocUsage( int num, int sizeofType);
void freeUsage( void *pointer);


