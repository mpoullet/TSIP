/*****************************************************
 * File..: CLI.h
 * Desc..: prototypes and includes for the console operation
 * Author: Michael Stegemann
 * Date..: 30.10.2012
 * 17.03.2014 Strutz: use wchar_t in SessionParameters
 * 19.05.2014 Strutz: BPCsFlag
 * 26.08.2015 int instead of cbool for modif_flag
 * 30.11.2015 new: SCF_predcition, SCF_stage2tuning
 * 24.08.2016 new: SCF_maxNumPattern
 * 14.11.2016 include locoImproveFlag
 * 15.08.2017 include LSsearchSpace, LSnumOfPreds
 * 07.09.2017 LSuseColParal_flag
 ******************************************************/

#pragma once

#include "Form1.h"
#include "stdafx.h"
#include <string.h>

/* for the usage of gcroot*/
#include <vcclr.h>


namespace bacoder
{
ref class Form1;
}

typedef struct Parameter
{
    gcroot<System::String^> value;
    bool isPassed;
} Parameter;

typedef struct cliArguments
{
  Parameter
    log_name,
		tileWidth,
		tileHeight,
		postbwt,
    postBWTparam,
    MTF_val,
    IFCresetVal,
    IFCmaxCount,
    rct_type,
    indexed_flag,
    indexed_adj,
    indexed_arith,
    palette_sorting,
    skip_precoding,
    skip_rlc1,
    skip_postBWT,
    use_RLC2zero,
    segmentWidth,
    segmentHeight,
    entropy_coding_method,
    separate_coding_flag,
    coding_method,
    interleaving_mode,
    //T4_flag,
		locoImproveFlag,
    predMode,
    histMode;

    // settings of the H.26x Predictor
    Parameter
	mse_thresh,
    logStatistics,
    printErrorPic,
    h_e_predictors,
    h_d_predictors,
    blockSizes,
    postTransform;

    // settings of the Adapt Predictor
    Parameter
    mode,
    contextFile,
    blockWidth,
    blockHeight,
    cxPx,
    cxGrad,
    a_predictors,
    a_bias,
    sigma,

		LSsearchSpace,
		LSnumOfPreds,
		LSuseCorr_flag, LSuseColPred_flag, LSuseColParal_flag,

		CoBaLP2_sT,
		CoBaLP2_sA,
		CoBaLP2_sB,
		CoBaLP2_aR,
		CoBaLP2_aL,
		CoBaLP2_constLines,

		BPCsFlag,

		HXCauto,
		HXCtolerance,
	 HXCtoleranceOffset1,	/* for diagonal neighbours C and D	*/
	 HXCtoleranceOffset2,	/* for  neighbours E and F	*/
	 HXCincreaseFac,

		HXC2auto,
	  HXC2increaseFac,
		HXC2tolR0,
		HXC2tolR1,
		HXC2tolR2,
		HXC2tolR3,
		HXC2tolR4,
		HXC2tolR5,
		HXC2tolG0,
		HXC2tolG1,
		HXC2tolG2,
		HXC2tolG3,
		HXC2tolG4,
		HXC2tolG5,
		HXC2tolB0,
		HXC2tolB1,
		HXC2tolB2,
		HXC2tolB3,
		HXC2tolB4,
		HXC2tolB5,
		/* SCF parameters	*/
		SCF_directional, /* histo modification based on same pattern detection	*/
		SCF_prediction, /* quadtree-based segmentation	*/
		SCF_stage2tuning, /* modification of palett distribution	*/
		SCF_colourMAP,	/* re-use best Y prediction mode for U, V */
		SCF_maxNumPatternsIdx; /* maximal elements in each pattern sub-list	*/
} cliArguments;

typedef struct
{
    cliArguments cli;
    PicData *pD;
    // strutz char *inFN, *outFN;
		wchar_t *inFN, *outFN;
} SessionParameters;



void initArgumentList(cliArguments* cli);
void passArgumentToList(cliArguments* cli,System::String^ key, System::String ^ value);
SessionParameters* parseArgumentString(array<System::String ^> ^argumente);
int applyArgumentList(cliArguments* cli, AutoConfig* aC);
cbool applyArgumentList(cliArguments* cli, H26xSettings* settings);
cbool applyArgumentList(cliArguments* cli, CustomParam* cparam);

void consoleOperation(SessionParameters* sP);