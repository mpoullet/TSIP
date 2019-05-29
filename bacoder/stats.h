/*****************************************************
* Datei: stats.h
* Desc.: Header für Statistik-Funktionen
* Autor: Thomas Schmiedel, T.Strutz
* Datum: 02.11.2010, 13.5.2011, 09.07.2011
* 06.07.2012 prototyping for heap_sort_ul()
* 10.07.2012 prototyping for heap_sort_ull()
* 11.07.2012 PALETTE_THRESHOLD
* 25.02.2014 new: getColourSaturation()
* 25.02.2014 new: getColourContrast()
 * 13.06.2014 new declaration of heap_sort()
 * 18.07.2014 new getRepeatness()
 * 10.10.2014 new getHaarFeatures()
 * 16.11.2015 new: heap_sort_ll()
 * 27.07.2018 added num_contexts in structure
******************************************************/

#ifndef _INDEXSTATS_H_
#define _INDEXSTATS_H_

#include "tstypes.h"
#include "ginterface.h"
#include "imageFeatures.h" /* for HAARFEAT	*/

#define PALETTE_THRESHOLD	1500 /* decides method for palette coding	*/


/* Statistikfunktionen für uint-Arrays */
double GetEntropy(uint *, uint);
ulong GuessSizeBytes(uint *, uint);

/* Statistikfunktionen für Bilder */
double PicEntropy( PicData *);
uint HowSynthetic( PicData *, unsigned int syn[4]);
unsigned int getRepeatness( PicData *pd);
//void getHaarFeatures( PicData *pd, HAARFEAT *features);

unsigned int getColourContrast( PicData *pd);
unsigned int getColourSaturation( PicData *pd);
unsigned int GetRoughness( PicData *pd);
unsigned int GetDisruption( PicData *pd);
unsigned int GetCorrelation( PicData *pd);
unsigned int getMonotony( PicData *pd);
unsigned long GetNumColours( PicData *pd, unsigned long *maxCount,
														double *colEntropy);

unsigned long GetNumColoursBlock( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end, 
														 //unsigned long *maxCount,
														double *colEntropy);
void GetPatEntropy( PicData *pd, 
										 unsigned int y_start, unsigned int y_end,
										 unsigned int x_start, unsigned int x_end,
										 double entropy[3]);
unsigned long GetIndexedColours( PicData *, 
											unsigned char *palette);
void Indexed2RGBColours( PicData *pd, unsigned char *palette, 
												unsigned int numColours);
//CSpace  PCA_3D( PicData *pd);

/* Farbräume testen */
uint CountGray( PicData *pd);
//CSpace TestModes( PicData *, unsigned int numColours, float tune_YUV_val,
//								 int *flag);


typedef struct { 
	float entropy[3]; 
	unsigned int data_len[3]; 
	unsigned int RLC1_len[3]; 
	unsigned int RLC1_marker[3]; 
	unsigned int RLC2_len[3]; 
	unsigned int RLC2_marker[3]; 
	unsigned short num_contexts;
	unsigned short num_contextsLS;
} INTERMEDIATE_VALS;

void heap_sort(unsigned long N, unsigned long ra[]);
void heap_sort_ll(unsigned long N, unsigned long long ra[]);

void
heap_sort_uc(unsigned long N, unsigned char ra[],
	unsigned long pos_ptr[]);
void
heap_sort_ul(unsigned long N, unsigned long ra[],
	unsigned long pos_ptr[]);
void
heap_sort_ull(unsigned long N, unsigned long long ra[],
	unsigned long pos_ptr[]);


#endif

