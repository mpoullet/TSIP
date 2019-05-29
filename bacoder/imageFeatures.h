/*****************************************************************
 *
 * File....: imageFeatures.h
 * Purpose.: declaration of feature structure
 * Date....: 09.02.2012
 * Author..: Tilo Strutz
 * changes
 * 17.02.2012 add estimateNoise()
 * 08.02.2014 add constance
 * 25.02.2014 new: colourSaturation
 * 04.04.2014 new: rgbStdDev[3]
 * 04.07.2014 element 'flag' in structure deleted
 * 18.07.2014 new element: repeatness
 * 23.01.2015 syn2
 * 02.02.2015 new: get_syntheticness()
 * 04.05.2016 new: colEntropy
 * 26.08.2016 new: joint entropies 
 *****************************************************************/
#ifndef IMAGEFEATURES_H
#define IMAGEFEATURES_H

#include "ginterface.h"

//typedef struct {
//	int entropy_AD1, entropy_DA1, entropy_DD1;
//	int sum_AD1, sum_DA1, sum_DD1;
//	int sum_AD2, sum_DA2, sum_DD2;
//	int sum_AD3, sum_DA3, sum_DD3;
//} HAARFEAT;

typedef struct {
	unsigned int
		numColours,
	  syn, 
	  syn2, /* after histogram modification	*/
	  syn_grad, /* max four components	*/ 
		constance, /* identical neighbours	*/
		corrcoeff[4],
		monotony[4], 
		noise[4],	/* estimation of noisy content	*/
		colModalPart,	/* percentage of modal value of colors	*/
		colourContrast,
		relColourContrast,
		colourSaturation,
		relColourSaturation,
		rgbStdDev[3];
	float		colperpix;
	double colEntropy;
	double jointEntropy[3];
	unsigned long		maxDepth;
	unsigned long		patternMeasure;
	//HAARFEAT HaarFeatures;
} ImageFeatures;

void getImageFeatures( PicData *curpic, ImageFeatures *iF, 
											int coding_method_isPassed, 
											int coding_method);
unsigned long estimateNoise( PicData *pd, ImageFeatures * iF);
void get_syntheticness( PicData *pd, ImageFeatures * iF);

/*------------------------------------------------------------------*/
#endif

