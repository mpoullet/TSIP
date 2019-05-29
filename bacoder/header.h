/*****************************************************
* Datei: header.h
* Autor: Tilo Strutz
* Datum: 27.03.2014
* Desc.: prototyping for Header routines
* 04.07.2014 support of transparency
 * 10.11.2015 new: SCF_directional
 * 30.11.2015 new: SCF_predcition,SCF_stage2tuning, SCF_colourMAP
 * 26.08.2016 new: SCF_maxNumPatterns(Idx) has to be transmitted
 * 15.08.2017 new 	LSpred_searchSpace;	LSpred_numOfPreds, useCorrelation_flag
 * 07.09.2017 new useColPredictionP_flag
******************************************************/
#ifndef _HEADER_H_
#define _HEADER_H_

#include<bitbuf.h>
#include<autoConfig.h>

/* Headerstruktur,enthält 
essentielle Bildinformationen */
typedef struct
{
	/* general parameters	*/
	unsigned int 
		 channel,
		 width,
		 height,
		 //bitperchannel,
		 bitperchan[4],
		 maxVal_orig[4],
		 maxVal_rct[4],
		 maxVal_pred[4],
		 offset[4],
		 colour_space;
	unsigned int palette_sorting;
	// HistModeType histMode;
	int histMode;
	//CompMode	 coding_method;
	int	 coding_method;

	/* TSIP / BPC only parameters	*/
	//unsigned int  segmentWidth,	segmentHeight;
	unsigned int /* Only valid when indexed_flag is true */
		indexed_adj, /* Use adjacency mapping when 1 */
		indexed_arith; /* Use direct arithmetic coding when 1 */
	unsigned int 		 pbwtparam;
	PredictionType 	 predMode;
	PostBWT			 postbwt;
	unsigned int	 maxIFCCount;
	InterleavingType	 interleaving_mode;
	//int	 interleaving_mode;

	int	 entropy_coding_method;
	int	 separate_coding_flag;
	uint skip_precoding;
	uint skip_rlc1;
	uint skip_postBWT;
	uint use_RLC2zero;
	H26xSettings* H26x_Set;

	/* HXC only parameters	*/
	unsigned int HXCtolerance;
	unsigned int HXCtoleranceOffset1;	/* for diagonal neighbours C and D	*/
	unsigned int HXCtoleranceOffset2;	/* for  neighbours E and F	*/
	unsigned int HXCincreaseFac;

	unsigned int HXC2tolerance[4][10];	/* for all positions	*/
	unsigned int HXC2increaseFac;
	unsigned int pattern_size;
	/* SCF */
	unsigned int SCF_directional;
	unsigned int SCF_prediction;
	unsigned int SCF_stage2tuning;
	unsigned int SCF_colourMAP;
	unsigned int SCF_maxNumPatternsIdx;
	//PM_MA improvement 01 count number of colours added rotation information
	unsigned int SCF_rotationInfo_bool_01;
	unsigned int SCF_rotationInfo_bool_02;
	//PM_MA for improved stage 2 tuning
	unsigned int SCF_paletteSizeStage2Tuning_bool_01;
	unsigned int SCF_paletteSizeStage2Tuning_bool_02;
	//unsigned int SCF_diffMaxCalculated_bool_01;
	//unsigned int SCF_diffMaxCalculated_bool_02;
	//unsigned int SCF_diffMaxCalculated_bool_03;
	unsigned int SCF_diffMaxCalculated;

	/* BPC	*/
	unsigned char	s_flg;
	unsigned char use_colFlag;	/*  flag for BPC, check RCT?	*/
	unsigned char use_predFlag;	/* flag for BPC, check prediction?	*/

	int transparency;	/**/
	unsigned int transparency_colour;

	/* CoBaLP2	*/
	unsigned char wA, wB; /* 10x abs(weight)	*/

	/* TSIP LS prediction	*/
	unsigned char LSpred_searchSpace,	LSpred_numOfPreds;
	unsigned char useCorrelation_flag, useColPrediction_flag, useColPredictionP_flag;

} PicHeader;

/* Header erzeugen, löschen, speichern, laden */ 
PicHeader *CreateHeader();
void DeleteHeader( PicHeader *ph);
void WriteHeader( PicHeader *ph, BitBuffer *bitbuf);
PicHeader *LoadHeader(BitBuffer *);


#endif

