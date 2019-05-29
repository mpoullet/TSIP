/************************************************
 *
 * File...:	compaction.h
 * Descr..:	Definitionens for histo_compaction.c
 * Author.:	Tilo Strutz
 * Date...: 24.05.2013
 * changes:
 * 26.07.2013 external determination of image histogram
 * 10.09.2013 compact_histogram() type of histo changed 
 ************************************************/
#ifndef COMPACTION_H
#define COMPACTION_H

#include "bitbuf.h"
#include "types_CoBaLP.h"

//unsigned int compact_histogram( IMAGE *im, 
//							 long *lptr, 
//							 unsigned int hist_len,
//							 unsigned int *idx2val);
unsigned int compact_histogram( IMAGE *im, 
							 long *lptr, 
							 	unsigned long *histo,
								unsigned int hist_len,
							 unsigned int *idx2val);

void restore_histogram( IMAGE *im, long *lptr, 
							 unsigned int *idx2val);

void write_compaction_LUT( IMAGE *im, unsigned int *idx2val, 
													unsigned int new_max_val,
													BitBuffer *bitbuf);

unsigned int read_compaction_LUT( IMAGE *im, unsigned int *idx2val, 
													unsigned int new_max_val,
													BitBuffer *bitbuf);

#endif
