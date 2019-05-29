/*****************************************************************
*
* File....:	autoConfig.h
* Purpose.:	declaration of Config structure
* Date....: 	03.02.2012
* Author..: Tilo Strutz
* changes
* 08.02.2012 new element indexed_flag
* 24.07.2012 new element palette_sorting
* 27.07.2012 new elements use_RLC2zero,indexed_adj,indexed_arith
* 02.08.2013 CoBaLP2 in CompMode
 * 27.08.2013 type CompMode for ac->compression_mode
 * 02.09.2013 new HCXmode
 * 08.11.2013 aC->postBWTparam removed
 * 31.01.2014 new Parameters for HXC in aC->
 * 10.02.2014 new HXCauto 
 * 19.05.2014 new aC->s_flag
 * 01.07.2014 support of tiles
 * 20.08.2014 support HXC2
 * 23.06.2014 new type CompressionSettings
 * 02.12.2014 new flags: 	aC->use_colFlag,	aC->use_predFlag 
 * 29.07.2015 new flag: 	aC->rct_manual_flag set 
 * 25.05.2015 substitute HistMode by int, as the values cannot be
 *             reliably determined
 * 03.09.2015 new: HXC3 for TSIP mode
 * 10.11.2015 new: SCF_directional
 * 30.11.2015 new: SCF_prediction, stage2tuning, SCF_colourMAP
 * 22.08.2016 new: SCF_maxNumPatterns, RCTpercentage
 * 14.11.2016 include locoImproveFlag
 * 15.08.2017 new 	LSpred_searchSpace;	LSpred_numOfPreds, useCorrelation_flag
 * 07.09.2017 new useColPredictionP_flag
 *****************************************************************/
#ifndef AUTOCONFIG_H
#define AUTOCONFIG_H

#include "imageFeatures.h"
#include "ginterface.h"
//#include "hxc.h"	/* for MAX_PATTERN_SIZE	*/


#define NUM_CLASSES	30
#define NUM_FEATURES	8


struct H26xSettings;

/* enumeration of histogram modification types */
#ifdef ENUM_DOES_NOT_WORK_PROPERLY
typedef enum
{
	NoMOD = 0,
	COMPACTION,
	PERMUTATION,
	PERMUTE_2OPT,
	C_RESET,	/* compaction  was selected, 
				 * but programm decided that it is not worth to do it*/
	P_RESET,	/*  permutation was selected, 
				 * but programm decided that it is not worth to do it*/
	T_RESET	/*  permutation_TSP was selected, 
				 * but programm decided that it is not worth to do it*/
} HistModeType;
#endif

#define	NoMOD  0
#define	COMPACTION 1
#define	PERMUTATION 2
#define	PERMUTE_2OPT 3
#define	C_RESET 4	/* compaction  was selected, 
				 * but programm decided that it is not worth to do it*/
#define	P_RESET 5	/*  permutation was selected, 
				 * but programm decided that it is not worth to do it*/
#define	T_RESET 6	/*  permutation_TSP was selected, 
				 * but programm decided that it is not worth to do it*/

/* prediction modes */
typedef enum
{
	NONE = 0,
	LEFTNEIGHBOUR,
	PAETH,
	LSBLEND,
	LMSPRED,
	LSPRED,
	MED,
	MEDPAETH,
	ADAPT,
	H26x
} PredictionType;

/* post BWT method*/
typedef enum
{
	MTF = 0,
	IFC,
	HXC3
} PostBWT;

typedef enum
{
	YuvYuv,
	Y_U_V = 1,
	YY_uvuv,
	YY_uuvv,
	YYuuvv
} InterleavingType;
/* due to some problems now as defines	*/
//#define		Y_U_V		0
//#define 	YY_uvuv	1
//#define 	YY_uuvv	2
//#define 	YYuuvv	3
//#define 	YuvYuv	4


#ifdef ENUM_DOES_NOT_WORK_PROPERLY
typedef enum
{
	LOCO = 0,
	TSIP,
	TSIPindexed,
	HXC,
	HXC2,
	BPC,
	CoBaLP2,
	MiXeD		/* when using tiles the method can be different for tiles	*/
} CompMode;	/* compression mode	*/
#endif
/* compression mode	*/
#define	LOCO		0
#define	TSIP		1
#define	TSIPindexed	2
#define	HXC			3
#define	HXC2		4
#define	BPC			5
#define	CoBaLP2	6
#define	MiXeD		7	/* when using tiles the method can be different for tiles	*/
#define	SCF	8

typedef enum
{
	setBPC = 0,
	setBPC_COMP,
	setBPC_Idx,
	setBPC_Idx_PERMUT,
	setBPC_PAETH,
	setBPC_ADAPT,
	setHXC_Idx,
	setHXC,
	setHXC2_auto,
	setHXC2_Tol0,
	setHXC2_MED,
	setSCF_auto,
	setSCF_zero_3,
	setSCF_0_20_220,
	setSCF_10_40_60_1,
	setTSIP_MTF,
	setTSIP_10_3_skip0,
	setTSIP_Idx,
	setTSIP_Idx_80_5,
	setTSIP_Idx_10_3,
	setTSIP_Idx_10_3_skip0,
	setTSIP_Idx_120_80_skip0,
	setTSIP,
	setTSIP_ADAPT,
	setTSIP_MEDPAETH,
	setLOCO,
	setLOCO_T4,
	setCoBaLP2_Idx,
	setCoBaLP2_Idx_PERMUT,
	setCoBaLP2,  /* constant lines off */
	setCoBaLP2_cl,  /* constant lines on */
	setCoBaLP2_TM		/* use Template Matching '-sT 0' */
} CompressionSettings;


typedef struct AutoConfig
{
	char *log_name;
	PostBWT
		postbwt;	/* method after BWT  MTF or IFC	*/
	unsigned int
		// postBWTparam,	/* MTF_val or IFCresetVal	*/
		MTF_val,	/* parameter for MoveToFront	*/
		IFCresetVal,	/* reset parameter for incremental frequency count	*/
		IFCmaxCount;	/*  parameter for incremental frequency count	*/
	unsigned char
		RCTpercentage,
		rct_manual_flag,
		rct_type_used,		/* chosen reversible colour transform	*/
		rct_type_pred,		/* chosen reversible colour transform	*/
		rct_type_nopr;		/* alternative reversible colour transform	with/without prediction*/
	unsigned char
		indexed_flag;		/* use indexed colours	*/
	unsigned int /* Only valid when indexed_flag is true */
		indexed_adj, /* Use adjacency mapping when 1 */
		indexed_arith; /* Use direct arithmetic coding when 1 */
	unsigned char
		palette_sorting;		/* 0 ... Green-Red-Blue
								* 1 ... luminance based
								*/
	unsigned char
		skip_precoding,	/* excludes entire precoding stage	*/
		skip_postBWT,	/* neither do MTF nor IFC	*/
		skip_rlc1,		/* excludes runlength coding before BWT	*/
		//skip_mtf,			/* excludes ranking after BWT	*/
		use_RLC2zero;	/* run length of zeros fpr RLC2	*/
	unsigned long
		tileWidth,		/* width + height of tiles	*/
		tileHeight,	
		maxTileWidth,
		maxTileHeight;
	unsigned char
		entropy_coding_method,	/*  0 ... Huffman coding
														 *  1 ... arithmetic coding
														 */
		separate_coding_flag;	/*  0 ... jointly coding of symbols and runs
														*  1 ... separate coding
														*/
	//CompMode
	int	coding_method;	/*  flag for algorithm,  TSIP, LOCO, BPC,  CoBaLP2 a.s.o.	*/
	InterleavingType
		interleaving_mode;	/* Y|U|V  YY|uvuv etc	*/
	unsigned char
		locoImproveFlag,
		T4_flag;
	PredictionType	predMode;		/* chosen prediction mode	*/
	/* HistModeType	histMode;		/* histogram compaction?	*/
	int	histMode;		/* histogram compaction?	*/
	H26xSettings* H26x_Set;

	unsigned int HXCauto;	/* allow internal automatic settings, overwriting manual ones*/
	unsigned int HXCtolerance;	/* for direct neighbours A and B	*/
	unsigned int HXCtoleranceOffset1;	/* for diagonal neighbours C and D	*/
	unsigned int HXCtoleranceOffset2;	/* for  neighbours E and F	*/
	unsigned int HXCincreaseFac;
	unsigned long num_HXC_patterns;

	//unsigned int HXC2tolerance[4][PATTERN_SIZE];	/* for all positions	*/
	unsigned int HXC2auto;	/* allow internal automatic settings, overwriting manual ones*/
	unsigned int HXC2tolerance[4][10];	/* for all positions	*/
	unsigned int HXC2increaseFac;
	unsigned int pattern_size;
	unsigned int SCF_directional; /* histo modification based on same pattern detection	*/
	unsigned int SCF_prediction; /* quadtree-based segmentation	*/
	unsigned int SCF_stage2tuning; /* quadtree-based segmentation	*/
	unsigned int SCF_colourMAP; /* use MAP prediction mode, which is best for Y	*/
	unsigned int SCF_maxNumPatterns; /* true value */
	unsigned int SCF_maxNumPatternsIdx; /* 1, 2,, ..., 8	*/

	unsigned char CoBaLP2_sT;	/* skip template matching in CoBaLP2	*/
	unsigned char CoBaLP2_sA;	/* skip predictor A in CoBaLP2	*/
	unsigned char CoBaLP2_sB;	/* skip predictor B in CoBaLP2	*/
	unsigned char CoBaLP2_aR;	/* enable predictor R in CoBaLP2	*/
	unsigned char CoBaLP2_aL;	/* enable predictor LS in CoBaLP2	*/
	unsigned char CoBaLP2_constLines;	/* enables compression of constant lines	*/

	unsigned char LSpred_searchSpace;	/* 	*/
	unsigned char LSpred_numOfPreds;	/* 	*/
	unsigned char useCorrelation_flag;
	unsigned char useColPrediction_flag; /* enables cross-component prediction	*/
	unsigned char useColPredictionP_flag; /* enables parallel cross-component prediction	*/

	unsigned char s_flg;	/*significance flag for BPC	*/
	unsigned char use_colFlag;	/*  flag for BPC, check RCT?	*/
	unsigned char use_predFlag;	/* flag for BPC, check prediction?	*/
//	friend bool operator== (AutoConfig aC1, AutoConfig aC2);
	friend bool operator!= (AutoConfig aC1, AutoConfig aC2);

} AutoConfig;

void autoConfig( PicData *curpic, ImageFeatures *iF, 
					AutoConfig *aC, double entropie[], 
					int coding_method_isPassed);

void cloneAutoConfig( AutoConfig* o_aC, AutoConfig* c_aC);
int compare_aC( AutoConfig *aC1, AutoConfig *aC2);


unsigned int mapIdx2maxNumPatterns( unsigned int Idx);
/*------------------------------------------------------------------*/
#endif

