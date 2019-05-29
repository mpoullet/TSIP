/*****************************************************
* Datei: histMode.h
* Autor: Tilo Strutz
* Datum: 27.03.2014
* Desc.: prototyping for histogram modification
* changes:
* 28.08.2015 pred_flag
******************************************************/

#ifndef _HISTMODE_H_
#define _HISTMODE_H_

unsigned int compact_histogram( PicData *pd, 
															 int comp,
															 //unsigned int *max_val,
															 unsigned int *idx2val,
															 int pred_flag);

void restore_histogram_compact( PicData *pd, int comp,
											 unsigned int *idx2val);

unsigned int compact_histogram_X( PicData *pd, 
															 int comp,
															 unsigned int *idx2val,
															 unsigned int *val2idx2);

unsigned int permute_histogram( PicData *pd, 
															 int comp,
															 unsigned int length,
															 unsigned int *idx2val,
															 unsigned int *val2idx2);

unsigned int permute_histogram_kopt( PicData *pd, 
															 int comp,
															 unsigned int length,
															 unsigned int *idx2val,
															 unsigned int *val2idx2);

void restore_histogram_permute( PicData *pd, int comp,
											 unsigned int *idx2val);


#endif

