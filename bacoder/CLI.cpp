/*******************************************************************
 *
 * File....: CLI.c
 * Function: command-line interface 
 * Author..: Stegemann
 * Date....: 2012
 *
 * 02.08.2013 Strutz: interleaving mode with string input (not tested)
 * 27.08.2013 type CompMode for ac->compression_mode
 * 27.08.2013 use input filename for logging; bux fix for -interleaving
 * 27.08.2013 bux fix (TM may not be edisabled)
 * 31.01.2014 autoConfig() w/o return value
 * 04.02.2014 bugfix if (aC->rct_type == GREY) 
 * 10.02.2014 new HXCauto 
 * 12.02.2014 new: usage()
 * 10.03.2014 BitPlane as method
 * 18.03.2014 passing ^%outName to savePicFile() getting modified filename
 * 26.03.2014 bugfix minimum blockWidth is 2 or image width 
 * 08.04.2014 reset aC->rct_type_used to rct_type_pred if CoBaLP or LOCO are enabled
 * 08.04.2014 used ToLower() for key words and values (console parametzers)
 * 20.06.2014 bugfix: check whether an argument is given for last parameter
 * 01.07.2014 support of tiles
 * 20.08.2014 support HXC2 parameters
 * 01.10.2014 output Tile x of y tiles
 * 01.10.2014 if rct is passed, then set Idx to zero
 * 22.10.2014 bugfix, tilesize must be computed before autoConfig() is called for 
 *						the first time
 * 28.10.2014 bugfix, HXC2auto must be disabled as soon tolerances are given on cmd line 
 * 04.11.2014 bugfix, must be a string cli->indexed_flag.value = "0";
 * 06.03.2015 bugfix, if rct is set on command line, all 3 rct values must be set
 * 26.03.2015 passing numberOfTiles to log-routine
 * 12.05.2015 key -histMode tsp
 * 26.08.2015 int instead of cbool for modif_flag
 * 03.09.2015 new: HXC3 mode
 * 30.11.2015 new: SCF_predcition, SCF_stage2tuning, SCF_colourMAP
 * 24.08.2016 new: SCF_maxNumPatternsIdx
 * 07.09.2016 new: bugfix, modif_flag in consoleOperation()
 * 14.11.2016 include locoImproveFlag
 * 10.08.2017 new prediction mode for TSIP
 * 15.08.2017 include LSsearchSpace, LSnumOfpreds, LSuseCorr_flag
 * 22.08.2017 new LMSpred
 * 07.09.2017 '-colparal'  LSuseColParal_flag
 * 09.09.2017 bugfix:	int modif_flag = 0; in consoleOperation()
 * 14.08.2018 parameter aL for CoBaLP2
********************************************************************/
#include "CLI.h"
#include "tile.h" /* getTileSize() */

/* drom Form1.cpp	*/
void tsipWrapper_1( PicData *picdata, ImageFeatures *iF, 
							 AutoConfig *aC, double entropy[], 
							 int coding_method_isPassed);
int tsipWrapper_2( PicData *picdata, ImageFeatures *iF, 
							 AutoConfig *aC, CustomParam *cparam,
							 cliArguments* cli, int gui_flag, int *modif_flag);
TimeSpan tsipWrapper_3(  String ^ouputFileName,
											 String ^inputFileName,
											PicData* picdata,
											AutoConfig* aC,
											ImageFeatures* iF,
											INTERMEDIATE_VALS* iv,
											CustomParam* cparam,
											BYTES_STRUC *bytes,
											int modif_flag,
											int log_flag, char *log_name, String^% ct_string,
											int console_flag);

//String^ rct2string( AutoConfig* aC);
String^ rct2string( unsigned char rct_type);

/* cannot be moved to codec.h because of String^	*/
unsigned long  writeGlobalHeaderToFile( PicData* pD, uint numberOfTiles, 
															String^ filename);

using namespace System::IO;
using namespace System::Text::RegularExpressions;

//extern wchar_t outputFileName[512]; /* in Form1.cpp declared	*/

/*------------------------------------------------------------------
* void usage(void)
*
*-----------------------------------------------------------------*/
void usage(void)
{
		printf( "Supported options: \n");
		printf( " -help 1 \n");
		printf( " -log_name %%s (default: log_TSIP.txt \n");
		printf( " -i input_file (*.tsip, *.png, *.bmp, *.pnm, *.ppm, *pgm, ...)\n");
		printf( " -o output_file (*.tsip, *.png, *.bmp, *.ppm, *pgm, ...) \n");
		printf( " -rct ( 0..119; colour space ) \n");
		printf( " -Idx (1 ... use colour palette) \n");
		printf( " -Idx_sorting (0 ... Idx_GRB; 1 ... Idx_Lum) \n");
		printf( " -method (HXC, HXC2, TSIP, LOCO, LOCO_T4, CoBaLP2, BPC, SCF) \n");
		printf( " -histMode (comp, none, perm, kopt) \n");
		printf( " -pred (none, LEFT, PAETH, MED, MEDPAETH, ADAPT, H26x, LMSPRED, LSBLEND, LSPRED) \n");
		printf( " -searchSpace (2-20, default 9) \n");
		printf( " -numOfPreds (2-18, default 6) \n");
		printf( " -corr 0|1 (1 .. use correlation in LS blending) \n");
		printf( " -colPred 0|1 (1 .. use cross-colour prediction) \n");
		printf( " -colParal 0|1 (1 .. use parallel cross-colour prediction) \n");
		printf( " -tileWidth \n");
		printf( " -tileHeight \n");
		printf( "## BPC options ##  \n");
		printf( " -sFlag 1 include significance map in context determinantion \n");
		printf( "## LOCO options ##  \n");
		printf( " -improvedk 1 better estimation of coding parameter k \n");
		printf( "## HXC options ##  \n");
		printf( " -HXCauto (1 ... manual HXC settings will be ignored) \n");
		printf( " -tol (0...1; tolerance for HXC coding) \n");
		printf( " -tolOff1 (0...10; tolerance offset1 ) \n");
		printf( " -tolOff2 (0...20; tolerance offset2 ) \n");
		printf( " -HXCincreasefac (0...100; increase factor) \n");
		printf( "## HXC2 and SCF options ##  \n");
		printf( " -HXC2auto (1 ... manual HXC2 settings will be ignored) \n");
		printf( " -HXC2increasefac (0...100; increase factor) \n");
		printf( " -tolR0  \n");
		printf( " -tolR1  \n");
		printf( " -tolR2  \n");
		printf( " -tolR3  \n");
		printf( " -tolR4  \n");
		printf( " -tolR5  \n");
		printf( " -tolG0  \n");
		printf( " -tolG1  \n");
		printf( " -tolG2  \n");
		printf( " -tolG3  \n");
		printf( " -tolG4  \n");
		printf( " -tolG5  \n");
		printf( " -tolB0  \n");
		printf( " -tolB1  \n");
		printf( " -tolB2  \n");
		printf( " -tolB3  \n");
		printf( " -tolB4  \n");
		printf( " -tolB5  \n");
		printf( "## SCF options ##  \n");
		printf( " -SCF_directional (0, 1) detects vertical or horizontal structures \n");
		printf( " -SCF_prediction (0, 1) quadtree based segmentation \n");
		printf( " -SCF_stage2tuning (0, 1) MAP based modification of palette distribution\n");
		printf( " -SCF_colourMAP (0, 1) use best Y MAP predictor also for U and best U predictor for V\n");
		printf( " -SCF_maxNumPatt 1..8  maximum number of patterns per sub-list\n");
		printf( "## CoBaLP2 options ##  \n");
		printf( " -sT 1 (skip template matching) \n");
		printf( " -sA 1 (skip predictor A) \n");
		printf( " -sB 1 (skip predictor B) \n");
		printf( " -aR 1 (enable predictor R) \n");
//		printf( " -aL 1 (enable predictor LS) \n");
		printf( " -constLines 0 (disable constant lines) \n");
		printf( "## TSIP options ##  \n");
		//printf( " -Idx_adj (1, requires Idx) \n");
		//printf( " -Idx_arith (1, requires Idx_adj) \n");
		//printf( " -segWidth () \n");
		//printf( " -segHeight () \n");
		printf( " -interleaving (Y_U_V, YY_uuvv, YY_uvuv, YuvYuv, YYuuvv) \n");
		printf( " -skip_rlc1    (0 ... do not skip; 1 ... skip first run-length coding) \n");
		printf( " -skip_prec    (0 ... do not skip; 1 ... skip entire pre-coding stage) \n");
		printf( " -skip_postBWT (0 ... do not skip; 1 ... skip MTF/IFC) \n");
		printf( " -postBWT (MTF, IFC, HXC3) \n");
		printf( " -MTF_val (0 ... 100) \n");
		printf( " -IFCmax (1 ... 100) \n");
		printf( " -IFCreset (1 ... 10000) \n");
		printf( " -RLC2_zeros (1 ... combine zeros only) \n");
		printf( " -entropy_coding (0... Huffman; 1 ... arithmetic) \n");
		printf( " -separate (0... jointly coding; 1 ... separate coding \n");
		//printf( "H26x prediction: \n");
		//printf( " -mse_thresh \n");
		//printf( " -errorPicture \n");
		//printf( " -logStatistics (1... enable logging)\n");
		//printf( " -errorPicture (1... enable output)\n");
		//printf( " -enablePredictors (?)\n");
		//printf( " -disablePredictors \n");
		//printf( " -blockSizes \n");
		//printf( " -postTransform (1 ... enable transformation after prediction)\n");
		printf( "ADAPT prediction: \n");
		printf( " -blockWidth  (5 - 128) \n");
		printf( " -blockHeight (5 - 128) \n");
		printf( " -predictors (100000 ... use first predictor) \n");
		printf( "             (010000 ... use second predictor) \n");
		printf( "             (110000 ... use two first predictors) \n");
		printf( "             (101000 ... use first and third predictor) \n");
		printf( "             (011000 ... use second and third predictor) \n");
		printf( "             (011111 ... use all predictors but first) \n");
		//printf( " -bias (?)\n");
		//printf( " -sigma (for TM)\n");
		return;
}

/*------------------------------------------------------------------
* char* unMarshal(String^ string)
*
* a shortcut to turn managed String^´s into char* arrays.
* Return Value: A char array representing the String^ object
*-----------------------------------------------------------------*/
char* unMarshal( String^ string)
{
	return (char *)(void *)Marshal::StringToHGlobalAnsi( string);
}

/*------------------------------------------------------------------
* initArgumentList(cliArguments* cli)
*
* initalises the list of arguments for later usage
*-----------------------------------------------------------------*/
void initArgumentList( cliArguments* cli)
{
		cli->log_name.isPassed=
		cli->tileWidth.isPassed=
		cli->tileHeight.isPassed=
		cli->postbwt.isPassed =
		cli->postBWTparam.isPassed =
		cli->MTF_val.isPassed =
		cli->IFCresetVal.isPassed =
		cli->IFCmaxCount.isPassed =
		cli->rct_type.isPassed =
		cli->indexed_flag.isPassed =
		cli->indexed_adj.isPassed =
		cli->indexed_arith.isPassed =
		cli->palette_sorting.isPassed =
		cli->skip_precoding.isPassed =
		cli->skip_rlc1.isPassed =
		cli->skip_postBWT.isPassed =
		cli->skip_precoding.isPassed =
		cli->use_RLC2zero.isPassed =
		cli->segmentWidth.isPassed =
		cli->segmentHeight.isPassed =
		cli->entropy_coding_method.isPassed =
		cli->separate_coding_flag.isPassed =
		cli->coding_method.isPassed =
		cli->interleaving_mode.isPassed =
		//cli->T4_flag.isPassed =
		cli->locoImproveFlag.isPassed = 
		cli->predMode.isPassed =
		cli->LSsearchSpace.isPassed =
		cli->LSnumOfPreds.isPassed =
		cli->LSuseCorr_flag.isPassed = 
		cli->LSuseColPred_flag.isPassed = 
		cli->LSuseColParal_flag.isPassed = 
		cli->histMode.isPassed =

		cli->mse_thresh.isPassed =
		cli->logStatistics.isPassed =
		cli->printErrorPic.isPassed =
		cli->h_e_predictors.isPassed =
		cli->h_d_predictors.isPassed =
		cli->blockSizes.isPassed =
		cli->postTransform.isPassed =

		cli->BPCsFlag.isPassed =

		cli->HXCauto.isPassed =
		cli->HXCtolerance.isPassed =
		cli->HXCtoleranceOffset1.isPassed =
		cli->HXCtoleranceOffset2.isPassed =
		cli->HXCincreaseFac.isPassed =

		cli->HXC2auto.isPassed =
		cli->HXC2increaseFac.isPassed =
		cli->HXC2tolR0.isPassed =
		cli->HXC2tolR1.isPassed =
		cli->HXC2tolR2.isPassed =
		cli->HXC2tolR3.isPassed =
		cli->HXC2tolR4.isPassed =
		cli->HXC2tolR5.isPassed =
		cli->HXC2tolG0.isPassed =
		cli->HXC2tolG1.isPassed =
		cli->HXC2tolG2.isPassed =
		cli->HXC2tolG3.isPassed =
		cli->HXC2tolG4.isPassed =
		cli->HXC2tolG5.isPassed =
		cli->HXC2tolB0.isPassed =
		cli->HXC2tolB1.isPassed =
		cli->HXC2tolB2.isPassed =
		cli->HXC2tolB3.isPassed =
		cli->HXC2tolB4.isPassed =
		cli->HXC2tolB5.isPassed =

		cli->SCF_directional.isPassed =
		cli->SCF_prediction.isPassed =
		cli->SCF_stage2tuning.isPassed =
		cli->SCF_colourMAP.isPassed =
		cli->SCF_maxNumPatternsIdx.isPassed =

		cli->CoBaLP2_sT.isPassed =
		cli->CoBaLP2_sA.isPassed =
		cli->CoBaLP2_sB.isPassed =
		cli->CoBaLP2_aR.isPassed =
//		cli->CoBaLP2_aL.isPassed =
		cli->CoBaLP2_constLines.isPassed =

		cli->mode.isPassed =
		cli->contextFile.isPassed =
		cli->blockWidth.isPassed =
		cli->blockHeight.isPassed =
		cli->cxPx.isPassed =
		cli->cxGrad.isPassed =
		cli->a_predictors.isPassed =
		cli->a_bias.isPassed =
		cli->sigma.isPassed = false;
}

/*------------------------------------------------------------------
* passArgumentToList(cliArguments* cli,System::String^ key, 
*						System::String ^ value)
*
* adds the parameter <key> with <value> into the argument container
*-----------------------------------------------------------------*/
void passArgumentToList( cliArguments* cli,System::String^ key, System::String ^ value)
{
	printf( "Key: %s = %s\n", unMarshal(key), unMarshal(value));

	key = key->ToLower();

	if (key == "-log_name")
	{
		cli->log_name.value = value;
		cli->log_name.isPassed = true;
		return;
	}

	/* value of log_name may not be changed	*/
	value = value->ToLower();

	if (key == "-tilewidth")
	{
		cli->tileWidth.value = value;
		cli->tileWidth.isPassed = true;
		return;
	}
	if (key == "-tileheight")
	{
		cli->tileHeight.value = value;
		cli->tileHeight.isPassed = true;
		return;
	}
	if (key == "-rct")
	{
		cli->rct_type.value = value;
		cli->rct_type.isPassed = true;
		/* switch off indexed mode	*/
		cli->indexed_flag.value = "0";
		cli->indexed_flag.isPassed = true;
		return;
	}
	
	if (key == "-improvedk")
	{
		cli->locoImproveFlag.value = value;
		cli->locoImproveFlag.isPassed = true;
		return;
	}
	if (key == "-method")
	{
		cli->coding_method.value = value;
		cli->coding_method.isPassed = true;
		return;
	}
	if (key == "-histmode")
	{
		cli->histMode.value = value;
		cli->histMode.isPassed = true;
		return;
	}
	if (key == "-sflag")
	{
		cli->BPCsFlag.value = value;
		cli->BPCsFlag.isPassed = true;
		return;
	}
	if (key == "-hxcauto")
	{
		cli->HXCauto.value = value;
		cli->HXCauto.isPassed = true;
		return;
	}
	if (key == "-hxc2auto")
	{
		cli->HXC2auto.value = value;
		cli->HXC2auto.isPassed = true;
		return;
	}
	if (key == "-tol")
	{
		cli->HXCtolerance.value = value;
		cli->HXCtolerance.isPassed = true;
		return;
	}
	if (key == "-toloff1")
	{
		cli->HXCtoleranceOffset1.value = value;
		cli->HXCtoleranceOffset1.isPassed = true;
		return;
	}
	if (key == "-toloff2")
	{
		cli->HXCtoleranceOffset2.value = value;
		cli->HXCtoleranceOffset2.isPassed = true;
		return;
	}
	if (key == "-hxcincreasefac")
	{
		cli->HXCincreaseFac.value = value;
		cli->HXCincreaseFac.isPassed = true;
		return;
	}

	if (key == "-hxc2increasefac")
	{
		cli->HXC2increaseFac.value = value;
		cli->HXC2increaseFac.isPassed = true;
		return;
	}
	if (key == "-tolr0")
	{
		cli->HXC2tolR0.value = value; 	cli->HXC2tolR0.isPassed = true;		return;
	}
	if (key == "-tolr1")
	{
		cli->HXC2tolR1.value = value;		cli->HXC2tolR1.isPassed = true;		return;
	}
	if (key == "-tolr2")
	{
		cli->HXC2tolR2.value = value;		cli->HXC2tolR2.isPassed = true;		return;
	}
	if (key == "-tolr3")
	{
		cli->HXC2tolR3.value = value;		cli->HXC2tolR3.isPassed = true;		return;
	}
	if (key == "-tolr4")
	{
		cli->HXC2tolR4.value = value;		cli->HXC2tolR4.isPassed = true;		return;
	}
	if (key == "-tolr5")
	{
		cli->HXC2tolR5.value = value;		cli->HXC2tolR5.isPassed = true;		return;
	}
	if (key == "-tolg0")
	{
		cli->HXC2tolG0.value = value; 	cli->HXC2tolG0.isPassed = true;		return;
	}
	if (key == "-tolg1")
	{
		cli->HXC2tolG1.value = value;		cli->HXC2tolG1.isPassed = true;		return;
	}
	if (key == "-tolg2")
	{
		cli->HXC2tolG2.value = value;		cli->HXC2tolG2.isPassed = true;		return;
	}
	if (key == "-tolg3")
	{
		cli->HXC2tolG3.value = value;		cli->HXC2tolG3.isPassed = true;		return;
	}
	if (key == "-tolg4")
	{
		cli->HXC2tolG4.value = value;		cli->HXC2tolG4.isPassed = true;		return;
	}
	if (key == "-tolg5")
	{
		cli->HXC2tolG5.value = value;		cli->HXC2tolG5.isPassed = true;		return;
	}
	if (key == "-tolb0")
	{
		cli->HXC2tolB0.value = value;		cli->HXC2tolB0.isPassed = true;		return;
	}
	if (key == "-tolb1")
	{
		cli->HXC2tolB1.value = value;		cli->HXC2tolB1.isPassed = true;		return;
	}
	if (key == "-tolb2")
	{
		cli->HXC2tolB2.value = value;		cli->HXC2tolB2.isPassed = true;		return;
	}
	if (key == "-tolb3")
	{
		cli->HXC2tolB3.value = value;		cli->HXC2tolB3.isPassed = true;		return;
	}
	if (key == "-tolb4")
	{
		cli->HXC2tolB4.value = value;		cli->HXC2tolB4.isPassed = true;		return;
	}
	if (key == "-tolb5")
	{
		cli->HXC2tolB5.value = value;		cli->HXC2tolB5.isPassed = true;		return;
	}
	if (key == "-scf_directional")
	{
		cli->SCF_directional.value = value;		cli->SCF_directional.isPassed = true;		return;
	}
	if (key == "-scf_prediction")
	{
		cli->SCF_prediction.value = value;		cli->SCF_prediction.isPassed = true;		return;
	}
	if (key == "-scf_stage2tuning")
	{
		cli->SCF_stage2tuning.value = value;		cli->SCF_stage2tuning.isPassed = true;		return;
	}
	if (key == "-scf_colourmap")
	{
		cli->SCF_colourMAP.value = value;		cli->SCF_colourMAP.isPassed = true;		return;
	}
	if (key == "-scf_maxnumpatt")
	{
		cli->SCF_maxNumPatternsIdx.value = value;		cli->SCF_maxNumPatternsIdx.isPassed = true;		return;
	}

	if (key == "-st")
	{
		cli->CoBaLP2_sT.value = value;
		cli->CoBaLP2_sT.isPassed = true;
		return;
	}
	if (key == "-sa")
	{
		cli->CoBaLP2_sA.value = value;
		cli->CoBaLP2_sA.isPassed = true;
		return;
	}
	if (key == "-sb")
	{
		cli->CoBaLP2_sB.value = value;
		cli->CoBaLP2_sB.isPassed = true;
		return;
	}
	if (key == "-ar")
	{
		cli->CoBaLP2_aR.value = value;
		cli->CoBaLP2_aR.isPassed = true;
		return;
	}
/*	if (key == "-al")
	{
		cli->CoBaLP2_aL.value = value;
		cli->CoBaLP2_aL.isPassed = true;
		return;
	}
	*/
	if (key == "-constlines")
	{
		cli->CoBaLP2_constLines.value = value;
		cli->CoBaLP2_constLines.isPassed = true;
		return;
	}
	if (key == "-idx")
	{
		cli->indexed_flag.value = value;
		cli->indexed_flag.isPassed = true;
		return;
	}
	if (key == "-idx_sorting")
	{
		cli->palette_sorting.value = value;
		cli->palette_sorting.isPassed = true;
		return;
	}
	if (key == "-idx_adj")
	{
		cli->indexed_adj.value = value;
		cli->indexed_adj.isPassed = true;
		return;
	}
	if (key == "-idx_arith")
	{
		cli->indexed_arith.value = value;
		cli->indexed_arith.isPassed = true;
		return;
	}
	if (key == "-segwidth")
	{
		cli->segmentWidth.value = value;
		cli->segmentWidth.isPassed = true;
		return;
	}
	if (key == "-segheight")
	{
		cli->segmentHeight.value = value;
		cli->segmentHeight.isPassed = true;
		return;
	}
	if (key == "-interleaving")
	{
		cli->interleaving_mode.value = value;
		cli->interleaving_mode.isPassed = true;
		return;
	}
	if (key == "-pred")
	{
		cli->predMode.value = value;
		cli->predMode.isPassed = true;
		return;
	}
	if (key == "-searchspace")
	{
		cli->LSsearchSpace.value = value;
		cli->LSsearchSpace.isPassed = true;
		return;
	}
	if (key == "-numofpreds")
	{
		cli->LSnumOfPreds.value = value;
		cli->LSnumOfPreds.isPassed = true;
		return;
	}
	if (key == "-corr")
	{
		cli->LSuseCorr_flag.value = value;
		cli->LSuseCorr_flag.isPassed = true;
		return;
	}
	if (key == "-colpred")
	{
		cli->LSuseColPred_flag.value = value;
		cli->LSuseColPred_flag.isPassed = true;
		return;
	}
	if (key == "-colparal")
	{
		cli->LSuseColParal_flag.value = value;
		cli->LSuseColParal_flag.isPassed = true;
		return;
	}
	
	if (key == "-skip_rlc1")
	{
		cli->skip_rlc1.value = value;
		cli->skip_rlc1.isPassed = true;
		return;
	}
	if (key == "-skip_prec")
	{
		cli->skip_precoding.value = value;
		cli->skip_precoding.isPassed = true;
		return;
	}
	if (key == "-skip_postbwt")
	{
		cli->skip_postBWT.value = value;
		cli->skip_postBWT.isPassed = true;
		return;
	}
	if (key == "-postbwt")
	{
		cli->postbwt.value = value;
		cli->postbwt.isPassed = true;
		return;
	}
	if (key == "-mtf_val")
	{
		cli->MTF_val.value = value;
		cli->MTF_val.isPassed = true;
		return;
	}
	if (key == "-ifcmax")
	{
		cli->IFCmaxCount.value = value;
		cli->IFCmaxCount.isPassed = true;
		return;
	}
	if (key == "-ifcreset")
	{
		cli->IFCresetVal.value = value;
		cli->IFCresetVal.isPassed = true;
		return;
	}
	if (key == "-rlc2_zeros")
	{
		cli->use_RLC2zero.value = value;
		cli->use_RLC2zero.isPassed = true;
		return;
	}
	if (key == "-entropy_coding")
	{
		cli->entropy_coding_method.value = value;
		cli->entropy_coding_method.isPassed = true;
		return;
	}
	if (key == "-separate")
	{
		cli->separate_coding_flag.value = value;
		cli->separate_coding_flag.isPassed = true;
		return;
	}

	/* H.26x Part */

	if (key == "-mse_thresh")
	{
		cli->mse_thresh.value = value;
		cli->mse_thresh.isPassed = true;
		return;
	}

	if (key == "-logstatistics")
	{
		cli->logStatistics.value = value;
		cli->logStatistics.isPassed = true;
		return;
	}
	if (key == "-errorpicture")
	{
		cli->printErrorPic.value = value;
		cli->printErrorPic.isPassed = true;
		return;
	}
	if (key == "-enablepredictors")
	{
		cli->h_e_predictors.value = value;
		cli->h_e_predictors.isPassed = true;
		return;
	}
	if (key == "-disablepredictors")
	{
		cli->h_d_predictors.value = value;
		cli->h_d_predictors.isPassed = true;
		return;
	}
	if (key == "-blocksizes")
	{
		cli->blockSizes.value = value;
		cli->blockSizes.isPassed = true;
		return;
	}
	if (key == "-posttransform")
	{
		cli->postTransform.value = value;
		cli->postTransform.isPassed = true;
		return;
	}

	/* ADAPT Part */

	if (key == "-mode")
	{
		cli->mode.value = value;
		cli->mode.isPassed = true;
		return;
	}
	if (key == "-contextfile")
	{
		cli->contextFile.value = value;
		cli->contextFile.isPassed = true;
		return;
	}
	if (key == "-blockwidth")
	{
		cli->blockWidth.value = value;
		cli->blockWidth.isPassed = true;
		return;
	}
	if (key == "-blockheight")
	{
		cli->blockHeight.value = value;
		cli->blockHeight.isPassed = true;
		return;
	}
	if (key == "-cxpx")
	{
		cli->cxPx.value = value;
		cli->cxPx.isPassed = true;
		return;
	}
	if (key == "-cxgrad")
	{
		cli->cxGrad.value = value;
		cli->cxGrad.isPassed = true;
		return;
	}
	if (key == "-predictors")
	{
		cli->a_predictors.value = value;
		cli->a_predictors.isPassed = true;
		return;
	}
	if (key == "-bias")
	{
		cli->a_bias.value = value;
		cli->a_bias.isPassed = true;
		return;
	}
	if (key == "-sigma")
	{
		cli->sigma.value = value;
		cli->sigma.isPassed = true;
		return;
	}

	if (key == "-help")
	{
		usage( );
	}
	{
		char* str2 = (char*)(void*)Marshal::StringToHGlobalAnsi(key);
		fprintf(stderr, "This option (%s) is unknown. Ignored. \n", str2);
		Marshal::FreeHGlobal( IntPtr(str2));
	}
}

/*------------------------------------------------------------------
* SessionParameters* parseArgumentString(array<System::String ^> 
*  													^argumente)
*
* turns the provided argument list into a SessionParameters struct
* Return Value: If the arguments provided were sufficient a
*               SessionParameters struct with parameters for the run
*				 Else a NULL value.
*-----------------------------------------------------------------*/
SessionParameters* parseArgumentString( array<System::String ^> ^argumente)
{
	SessionParameters* sP = new SessionParameters;
	initArgumentList(&(sP->cli));

	sP->inFN = NULL; /* reset is important for test of validity later on	*/
	sP->outFN = NULL;
	//if (argumente->Length >= 4)
	if (argumente->Length >= 2)
	{
		for (int laufindex=0; laufindex < argumente->Length; laufindex=laufindex+2)
		{
			if (argumente[laufindex] == "-i")
			{
				sP->inFN = (wchar_t*)(void*)Marshal::StringToHGlobalUni( argumente[laufindex+1]);
			}
			else if (argumente[laufindex] == "-o")
			{
				sP->outFN = (wchar_t*)(void*)Marshal::StringToHGlobalUni( argumente[laufindex+1]);
			}
			else
			{
				if (laufindex+1 < argumente->Length)
				{
					passArgumentToList( &(sP->cli), argumente[laufindex], argumente[laufindex+1]);
				}
				else
				{
					fprintf( stderr, "### missing argument for option %s ###\n",
						unMarshal(argumente[laufindex]) );
				}
			}
		}
		return sP;
	}
	else
	{
		delete sP; /* free memory for SessionParameters    */ 
		return NULL;
	}
}

/*------------------------------------------------------------------
* int applyArgumentList(cliArguments* cli, AutoConfig* aC)
*
* applies the parameters provided via CLI to the AutoConfig struct
* for each tile
* Return Value: true (>0) if modifications were neccessary 
*-----------------------------------------------------------------*/
int applyArgumentList( cliArguments* cli, AutoConfig* aC)
{
	int modified = 0;
	//HistModeType tmpHist = NoMOD;
	int tmpHist = NoMOD;
	PredictionType tmpPred = NONE;
	InterleavingType tmpInterleav = Y_U_V;
	//int tmpInterleav = Y_U_V;
	//CompMode tmpCompMode;
	int tmpCompMode;
	PostBWT	tmpPostBWT;
	unsigned int tmpIntU;
	unsigned char tmpCharU = 0;


	if (cli->rct_type.isPassed == true)
	{
		if((String^)cli->rct_type.value == "rgb")
		{
			tmpCharU = 1;
			cli->rct_type.value = "1";	
			/* mapping of String to number, as extern routines compare numbers	*/
		}
		else if((String^)cli->rct_type.value == "hp1")
		{
			tmpCharU = 2;
			cli->rct_type.value = "2";
		}
		else if((String^)cli->rct_type.value == "a1_1")
		{
			tmpCharU = 2;
			cli->rct_type.value = "2";
		}
		else if((String^)cli->rct_type.value == "a1_2")
		{
			tmpCharU = 3;
			cli->rct_type.value = "3";
		}
		else if((String^)cli->rct_type.value == "a1_3")
		{
			tmpCharU = 4;
			cli->rct_type.value = "4";
		}
		else if((String^)cli->rct_type.value == "a1_4")
		{
			tmpCharU = 5;
			cli->rct_type.value = "5";
		}
		else if((String^)cli->rct_type.value == "a1_5")
		{
			tmpCharU = 6;
			cli->rct_type.value = "6";
		}
		else if((String^)cli->rct_type.value == "a1_6")
		{
			tmpCharU = 7;
			cli->rct_type.value = "7";
		}
		else if((String^)cli->rct_type.value == "a1_7")
		{
			tmpCharU = 8;
			cli->rct_type.value = "8";
		}
		else if((String^)cli->rct_type.value == "a1_8")
		{
			tmpCharU = 9;
			cli->rct_type.value = "9";
		}
		else if((String^)cli->rct_type.value == "a1_9")
		{
			tmpCharU = 10;
			cli->rct_type.value = "10";
		}
		else if((String^)cli->rct_type.value == "a1_10")
		{
			tmpCharU = 11;
			cli->rct_type.value = "11";
		}
		else if((String^)cli->rct_type.value == "a1_11")
		{
			tmpCharU = 12;
			cli->rct_type.value = "12";
		}
		else if((String^)cli->rct_type.value == "a1_12")
		{
			tmpCharU = 13;
			cli->rct_type.value = "13";
		}
		else if((String^)cli->rct_type.value == "a2_1")
		{
			tmpCharU = 14;
			cli->rct_type.value = "14";
		}
		else if((String^)cli->rct_type.value == "a2_2")
		{
			tmpCharU = 15;
			cli->rct_type.value = "15";
		}
		else if((String^)cli->rct_type.value == "a2_3")
		{
			tmpCharU = 16;
			cli->rct_type.value = "16";
		}
		else if((String^)cli->rct_type.value == "a2_4")
		{
			tmpCharU = 17;
			cli->rct_type.value = "17";
		}
		else if((String^)cli->rct_type.value == "a2_5")
		{
			tmpCharU = 18;
			cli->rct_type.value = "18";
		}
		else if((String^)cli->rct_type.value == "a2_6")
		{
			tmpCharU = 19;
			cli->rct_type.value = "19";
		}
		else if((String^)cli->rct_type.value == "a2_7")
		{
			tmpCharU = 20;
			cli->rct_type.value = "20";
		}
		else if((String^)cli->rct_type.value == "a2_8")
		{
			tmpCharU = 21;
			cli->rct_type.value = "21";
		}
		else if((String^)cli->rct_type.value == "a2_9")
		{
			tmpCharU = 22;
			cli->rct_type.value = "22";
		}
		else if((String^)cli->rct_type.value == "a2_10")
		{
			tmpCharU = 23;
			cli->rct_type.value = "23";
		}
		else if((String^)cli->rct_type.value == "a2_11")
		{
			tmpCharU = 24;
			cli->rct_type.value = "24";
		}
		else if((String^)cli->rct_type.value == "a2_12")
		{
			tmpCharU = 25;
			cli->rct_type.value = "25";
		}
		else if((String^)cli->rct_type.value == "a3_1")
		{
			tmpCharU = 26;
			cli->rct_type.value = "26";
		}
		else if((String^)cli->rct_type.value == "a3_2")
		{
			tmpCharU = 27;
			cli->rct_type.value = "27";
		}
		else if((String^)cli->rct_type.value == "a3_3")
		{
			tmpCharU = 28;
			cli->rct_type.value = "28";
		}
		else if((String^)cli->rct_type.value == "a3_4")
		{
			tmpCharU = 29;
			cli->rct_type.value = "29";
		}
		else if((String^)cli->rct_type.value == "a3_5")
		{
			tmpCharU = 30;
			cli->rct_type.value = "30";
		}
		else if((String^)cli->rct_type.value == "a3_6")
		{
			tmpCharU = 31;
			cli->rct_type.value = "31";
		}
		else if((String^)cli->rct_type.value == "a3_7")
		{
			tmpCharU = 32;
			cli->rct_type.value = "32";
		}
		else if((String^)cli->rct_type.value == "a3_8")
		{
			tmpCharU = 33;
			cli->rct_type.value = "33";
		}
		else if((String^)cli->rct_type.value == "a3_9")
		{
			tmpCharU = 34;
			cli->rct_type.value = "34";
		}
		else if((String^)cli->rct_type.value == "a3_10")
		{
			tmpCharU = 35;
			cli->rct_type.value = "35";
		}
		else if((String^)cli->rct_type.value == "a3_11")
		{
			tmpCharU = 36;
			cli->rct_type.value = "36";
		}
		else if((String^)cli->rct_type.value == "a3_12")
		{
			tmpCharU = 37;
			cli->rct_type.value = "37";
		}
		else if((String^)cli->rct_type.value == "a4_1")
		{
			tmpCharU = 38;
			cli->rct_type.value = "38";
		}
		else if((String^)cli->rct_type.value == "a4_2")
		{
			tmpCharU = 39;
			cli->rct_type.value = "39";
		}
		else if((String^)cli->rct_type.value == "a4_3")
		{
			tmpCharU = 40;
			cli->rct_type.value = "40";
		}
		else if((String^)cli->rct_type.value == "a4_4")
		{
			tmpCharU = 41;
			cli->rct_type.value = "41";
		}
		else if((String^)cli->rct_type.value == "a4_5")
		{
			tmpCharU = 42;
			cli->rct_type.value = "42";
		}
		else if((String^)cli->rct_type.value == "a4_6")
		{
			tmpCharU = 43;
			cli->rct_type.value = "43";
		}
		else if((String^)cli->rct_type.value == "a4_7")
		{
			tmpCharU = 44;
			cli->rct_type.value = "44";
		}
		else if((String^)cli->rct_type.value == "a4_8")
		{
			tmpCharU = 45;
			cli->rct_type.value = "45";
		}
		else if((String^)cli->rct_type.value == "a4_9")
		{
			tmpCharU = 46;
			cli->rct_type.value = "46";
		}
		else if((String^)cli->rct_type.value == "a4_10")
		{
			tmpCharU = 47;
			cli->rct_type.value = "47";
		}
		else if((String^)cli->rct_type.value == "a4_11")
		{
			tmpCharU = 48;
			cli->rct_type.value = "48";
		}
		else if((String^)cli->rct_type.value == "a4_12")
		{
			tmpCharU = 49;
			cli->rct_type.value = "49";
		}
		else if((String^)cli->rct_type.value == "a5_1")
		{
			tmpCharU = 50;
			cli->rct_type.value = "50";
		}
		else if((String^)cli->rct_type.value == "a5_2")
		{
			tmpCharU = 51;
			cli->rct_type.value = "51";
		}
		else if((String^)cli->rct_type.value == "a5_3")
		{
			tmpCharU = 52;
			cli->rct_type.value = "52";
		}
		else if((String^)cli->rct_type.value == "a5_4")
		{
			tmpCharU = 53;
			cli->rct_type.value = "53";
		}
		else if((String^)cli->rct_type.value == "a5_5")
		{
			tmpCharU = 54;
			cli->rct_type.value = "54";
		}
		else if((String^)cli->rct_type.value == "a5_6")
		{
			tmpCharU = 55;
			cli->rct_type.value = "55";
		}
		else if((String^)cli->rct_type.value == "a5_7")
		{
			tmpCharU = 56;
			cli->rct_type.value = "56";
		}
		else if((String^)cli->rct_type.value == "a5_8")
		{
			tmpCharU = 57;
			cli->rct_type.value = "57";
		}
		else if((String^)cli->rct_type.value == "a5_9")
		{
			tmpCharU = 58;
			cli->rct_type.value = "58";
		}
		else if((String^)cli->rct_type.value == "a5_10")
		{
			tmpCharU = 59;
			cli->rct_type.value = "59";
		}
		else if((String^)cli->rct_type.value == "a5_11")
		{
			tmpCharU = 60;
			cli->rct_type.value = "60";
		}
		else if((String^)cli->rct_type.value == "a5_12")
		{
			tmpCharU = 61;
			cli->rct_type.value = "61";
		}
		else if((String^)cli->rct_type.value == "a6_1")
		{
			tmpCharU = 62;
			cli->rct_type.value = "62";
		}
		else if((String^)cli->rct_type.value == "a6_2")
		{
			tmpCharU = 63;
			cli->rct_type.value = "63";
		}
		else if((String^)cli->rct_type.value == "a6_3")
		{
			tmpCharU = 64;
			cli->rct_type.value = "64";
		}
		else if((String^)cli->rct_type.value == "a6_4")
		{
			tmpCharU = 65;
			cli->rct_type.value = "65";
		}
		else if((String^)cli->rct_type.value == "a6_5")
		{
			tmpCharU = 66;
			cli->rct_type.value = "66";
		}
		else if((String^)cli->rct_type.value == "a6_6")
		{
			tmpCharU = 67;
			cli->rct_type.value = "67";
		}
		else if((String^)cli->rct_type.value == "a6_7")
		{
			tmpCharU = 68;
			cli->rct_type.value = "68";
		}
		else if((String^)cli->rct_type.value == "a6_8")
		{
			tmpCharU = 69;
			cli->rct_type.value = "69";
		}
		else if((String^)cli->rct_type.value == "a6_9")
		{
			tmpCharU = 70;
			cli->rct_type.value = "70";
		}
		else if((String^)cli->rct_type.value == "a6_10")
		{
			tmpCharU = 71;
			cli->rct_type.value = "71";
		}
		else if((String^)cli->rct_type.value == "a6_11")
		{
			tmpCharU = 72;
			cli->rct_type.value = "72";
		}
		else if((String^)cli->rct_type.value == "a6_12")
		{
			tmpCharU = 73;
			cli->rct_type.value = "73";
		}
		else if((String^)cli->rct_type.value == "a7_1")
		{
			tmpCharU = 74;
			cli->rct_type.value = "74";
		}
		else if((String^)cli->rct_type.value == "a7_2")
		{
			tmpCharU = 75;
			cli->rct_type.value = "75";
		}
		else if((String^)cli->rct_type.value == "a7_3")
		{
			tmpCharU = 76;
			cli->rct_type.value = "76";
		}
		else if((String^)cli->rct_type.value == "a7_4")
		{
			tmpCharU = 77;
			cli->rct_type.value = "77";
		}
		else if((String^)cli->rct_type.value == "a7_5")
		{
			tmpCharU = 78;
			cli->rct_type.value = "78";
		}
		else if((String^)cli->rct_type.value == "a7_6")
		{
			tmpCharU = 79;
			cli->rct_type.value = "79";
		}
		else if((String^)cli->rct_type.value == "a7_7")
		{
			tmpCharU = 80;
			cli->rct_type.value = "80";
		}
		else if((String^)cli->rct_type.value == "a7_8")
		{
			tmpCharU = 81;
			cli->rct_type.value = "81";
		}
		else if((String^)cli->rct_type.value == "a7_9")
		{
			tmpCharU = 82;
			cli->rct_type.value = "82";
		}
		else if((String^)cli->rct_type.value == "a7_10")
		{
			tmpCharU = 83;
			cli->rct_type.value = "83";
		}
		else if((String^)cli->rct_type.value == "a7_11")
		{
			tmpCharU = 84;
			cli->rct_type.value = "84";
		}
		else if((String^)cli->rct_type.value == "a7_12")
		{
			tmpCharU = 85;
			cli->rct_type.value = "85";
		}
		else if((String^)cli->rct_type.value == "a8_1")
		{
			tmpCharU = 86;
			cli->rct_type.value = "86";
		}
		else if((String^)cli->rct_type.value == "a8_2")
		{
			tmpCharU = 87;
			cli->rct_type.value = "87";
		}
		else if((String^)cli->rct_type.value == "a8_3")
		{
			tmpCharU = 88;
			cli->rct_type.value = "88";
		}
		else if((String^)cli->rct_type.value == "a8_4")
		{
			tmpCharU = 89;
			cli->rct_type.value = "89";
		}
		else if((String^)cli->rct_type.value == "a8_5")
		{
			tmpCharU = 90;
			cli->rct_type.value = "90";
		}
		else if((String^)cli->rct_type.value == "a8_6")
		{
			tmpCharU = 91;
			cli->rct_type.value = "7919";
		}
		else if((String^)cli->rct_type.value == "a8_7")
		{
			tmpCharU = 92;
			cli->rct_type.value = "92";
		}
		else if((String^)cli->rct_type.value == "a8_8")
		{
			tmpCharU = 93;
			cli->rct_type.value = "93";
		}
		else if((String^)cli->rct_type.value == "a8_9")
		{
			tmpCharU = 94;
			cli->rct_type.value = "94";
		}
		else if((String^)cli->rct_type.value == "a8_10")
		{
			tmpCharU = 95;
			cli->rct_type.value = "95";
		}
		else if((String^)cli->rct_type.value == "a8_11")
		{
			tmpCharU = 96;
			cli->rct_type.value = "96";
		}
		else if((String^)cli->rct_type.value == "a8_12")
		{
			tmpCharU = 97;
			cli->rct_type.value = "97";
		}
		else if((String^)cli->rct_type.value == "a9_1")
		{
			tmpCharU = 98;
			cli->rct_type.value = "98";
		}
		else if((String^)cli->rct_type.value == "a9_2")
		{
			tmpCharU = 99;
			cli->rct_type.value = "99";
		}
		else if((String^)cli->rct_type.value == "a9_3")
		{
			tmpCharU = 100;
			cli->rct_type.value = "100";
		}
		else if((String^)cli->rct_type.value == "a9_4")
		{
			tmpCharU = 101;
			cli->rct_type.value = "101";
		}
		else if((String^)cli->rct_type.value == "a9_5")
		{
			tmpCharU = 102;
			cli->rct_type.value = "102";
		}
		else if((String^)cli->rct_type.value == "a9_6")
		{
			tmpCharU = 103;
			cli->rct_type.value = "103";
		}
		else if((String^)cli->rct_type.value == "a9_7")
		{
			tmpCharU = 104;
			cli->rct_type.value = "104";
		}
		else if((String^)cli->rct_type.value == "a9_8")
		{
			tmpCharU = 105;
			cli->rct_type.value = "105";
		}
		else if((String^)cli->rct_type.value == "a9_9")
		{
			tmpCharU = 106;
			cli->rct_type.value = "106";
		}
		else if((String^)cli->rct_type.value == "a9_10")
		{
			tmpCharU = 107;
			cli->rct_type.value = "107";
		}
		else if((String^)cli->rct_type.value == "a9_11")
		{
			tmpCharU = 108;
			cli->rct_type.value = "108";
		}
		else if((String^)cli->rct_type.value == "a9_12")
		{
			tmpCharU = 109;
			cli->rct_type.value = "109";
		}
		else if((String^)cli->rct_type.value == "b1_1")
		{
			tmpCharU = 110;
			cli->rct_type.value = "110";
		}
		else if((String^)cli->rct_type.value == "b1_2")
		{
			tmpCharU = 111;
			cli->rct_type.value = "111";
		}
		else if((String^)cli->rct_type.value == "b2_1")
		{
			tmpCharU = 112;
			cli->rct_type.value = "112";
		}
		else if((String^)cli->rct_type.value == "b2_3")
		{
			tmpCharU = 113;
			cli->rct_type.value = "113";
		}
		else if((String^)cli->rct_type.value == "b3_2")
		{
			tmpCharU = 114;
			cli->rct_type.value = "113";
		}
		else if((String^)cli->rct_type.value == "b3_3")
		{
			tmpCharU = 115;
			cli->rct_type.value = "115";
		}
		else if((String^)cli->rct_type.value == "b4_1")
		{
			tmpCharU = 116;
			cli->rct_type.value = "116";
		}
		else if((String^)cli->rct_type.value == "b5_2")
		{
			tmpCharU = 117;
			cli->rct_type.value = "117";
		}
		else if((String^)cli->rct_type.value == "b6_3")
		{
			tmpCharU = 118;
			cli->rct_type.value = "118";
		}
		else if((String^)cli->rct_type.value == "pei09")
		{
			tmpCharU = 119;
			cli->rct_type.value = "119";
		}
		else
		{
			tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->rct_type.value));
		}
		
		if (tmpCharU > 0)
		{
			if (tmpCharU != aC->rct_type_used)
			{
				aC->rct_type_used = tmpCharU;
				aC->rct_type_nopr = tmpCharU;
				aC->rct_type_pred = tmpCharU;
				modified = 1;
			}
		}
		else
		{
			fprintf( stderr, "\n #### unknown rct (%d) !! ##\n", tmpCharU);
		}
	}

	if (cli->log_name.isPassed == true)
	{
		/* apply only the first time	*/
		if (aC->log_name == NULL)
		{
			aC->log_name = (char*)(void*)Marshal::StringToHGlobalAnsi( cli->log_name.value);
		}
	}
	if (cli->locoImproveFlag.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->locoImproveFlag.value));

		if (tmpIntU != aC->locoImproveFlag)
		{
			aC->locoImproveFlag = (unsigned char)tmpIntU;
			modified = 1;
		}
	}
	if (cli->coding_method.isPassed == true)
	{
		if((String^)cli->coding_method.value == "loco")
		{
			aC->HXCauto = 0; /* forces selected method	*/
			aC->HXC2auto = 0; /* forces selected method	*/
			tmpCompMode = LOCO;
			if (tmpCompMode != aC->coding_method)
			{
				aC->coding_method = tmpCompMode;
				modified = 1;
			}
			tmpCharU = 0;
			if (tmpCharU != aC->T4_flag)
			{
				aC->T4_flag = tmpCharU;
				modified = 1;
			}
			if (cli->rct_type.isPassed == false)
			{
				aC->rct_type_used = aC->rct_type_pred;
			}
		}
		else if((String^)cli->coding_method.value == "loco_t4")
		{
			aC->HXCauto = 0; /* forces selected method	*/
			aC->HXC2auto = 0; /* forces selected method	*/
			tmpCompMode = LOCO;
			if (tmpCompMode != aC->coding_method)
			{
				aC->coding_method = tmpCompMode;
				modified = 1;
			}
			tmpCharU = 1;
			if (tmpCharU != aC->T4_flag)
			{
				aC->T4_flag = tmpCharU;
				modified = 1;
			}
			if (cli->rct_type.isPassed == false)
			{
				aC->rct_type_used = aC->rct_type_pred;
			}
		}
		else if((String^)cli->coding_method.value == "bpc")
		{
			aC->HXCauto = 0; /* forces selected method	*/
			aC->HXC2auto = 0; /* forces selected method	*/
			tmpCompMode = BPC;
			if (tmpCompMode != aC->coding_method)
			{
				aC->coding_method = tmpCompMode;
				modified = 1;
			}
			if (cli->rct_type.isPassed == false)
			{
				aC->rct_type_used = aC->rct_type_nopr;
			}
		}
		else if((String^)cli->coding_method.value == "hxc")
		{
			tmpCompMode = HXC;
			if (tmpCompMode != aC->coding_method)
			{
				aC->coding_method = tmpCompMode;
				modified = 1;
			}
			if (cli->rct_type.isPassed == false)
			{
				aC->rct_type_used = aC->rct_type_nopr;
			}
		}
		else if((String^)cli->coding_method.value == "hxc2")
		{
			tmpCompMode = HXC2;
			if (tmpCompMode != aC->coding_method)
			{
				aC->coding_method = tmpCompMode;
				modified = 1;
			}
			if (cli->rct_type.isPassed == false)
			{
				aC->rct_type_used = aC->rct_type_nopr;
			}

		}
		else if((String^)cli->coding_method.value == "scf")
		{
			tmpCompMode = SCF;
			if (tmpCompMode != aC->coding_method)
			{
				aC->coding_method = tmpCompMode;
				modified = 1;
			}
			if (cli->rct_type.isPassed == false)
			{
				aC->rct_type_used = aC->rct_type_nopr;
			}

		}
		else if( (String^)cli->coding_method.value == "cobalp" ||
						 (String^)cli->coding_method.value == "cobalp2")
		{
			aC->HXCauto = 0; /* forces selected method	*/
			aC->HXC2auto = 0; /* forces selected method	*/
			tmpCompMode = CoBaLP2;
			if (tmpCompMode != aC->coding_method)
			{
				aC->coding_method = tmpCompMode;
				modified = 1;
			}
			if (cli->rct_type.isPassed == false)
			{
				aC->rct_type_used = aC->rct_type_pred;
			}
		}
		else	/* TSIP assumed	*/
		{
			aC->HXCauto = 0; /* forces selected method	*/
			aC->HXC2auto = 0; /* forces selected method	*/
			tmpCompMode = TSIP;
			if (tmpCompMode != aC->coding_method)
			{
				aC->coding_method = tmpCompMode;
				modified = 1;
			}
			if (cli->rct_type.isPassed == false)
			{
				aC->rct_type_used = aC->rct_type_nopr;
			}
		}
	}

	if (cli->histMode.isPassed == true)
	{
		if((String^)cli->histMode.value == "comp")
			tmpHist = COMPACTION;
		else if((String^)cli->histMode.value == "none")
			tmpHist = NoMOD;
		else if ((String^)cli->histMode.value == "perm")
			tmpHist = PERMUTATION;
		else if ((String^)cli->histMode.value == "kopt")
			tmpHist = PERMUTE_2OPT;
		else
			fprintf( stderr, "\n ### chosen histMode not supported ###");

		if (tmpHist != aC->histMode)
		{
			aC->histMode = tmpHist;
			modified = 1;
		}
	}
	if (cli->BPCsFlag.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi( unMarshal((String^)cli->BPCsFlag.value));

		if (tmpCharU != aC->s_flg)
		{
			aC->s_flg = tmpCharU;
			modified = 1;
		}
	}
	if (cli->HXCauto.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->HXCauto.value));

		if (tmpIntU != aC->HXCauto)
		{
			aC->HXCauto = tmpIntU;
			modified = 1;
		}
	}
	if (cli->HXC2auto.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->HXC2auto.value));

		if (tmpIntU != aC->HXC2auto)
		{
			aC->HXC2auto = tmpIntU;
			modified = 1;
		}
	}
	if (cli->HXCtolerance.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->HXCtolerance.value));

		if (tmpIntU != aC->HXCtolerance)
		{
			aC->HXCtolerance = tmpIntU;
			modified = 1;
		}
	}
	if (cli->HXCtoleranceOffset1.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->HXCtoleranceOffset1.value));

		if (tmpIntU != aC->HXCtoleranceOffset1)
		{
			aC->HXCtoleranceOffset1 = tmpIntU;
			modified = 1;
		}
	}
	if (cli->HXCtoleranceOffset2.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->HXCtoleranceOffset2.value));

		if (tmpIntU != aC->HXCtoleranceOffset2)
		{
			aC->HXCtoleranceOffset2 = tmpIntU;
			modified = 1;
		}
	}
	if (cli->HXCincreaseFac.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->HXCincreaseFac.value));

		if (tmpIntU != aC->HXCincreaseFac)
		{
			aC->HXCincreaseFac = tmpIntU;
			modified = 1;
		}
	}

	if (cli->HXC2increaseFac.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->HXC2increaseFac.value));

		if (tmpIntU != aC->HXC2increaseFac)
		{
			aC->HXC2increaseFac = tmpIntU;
			modified = 1;
		}
	}

	if (cli->HXC2tolR0.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolR0.value));
		if (tmpIntU != aC->HXC2tolerance[0][0])
		{
			aC->HXC2tolerance[0][0] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolR1.isPassed == true)
	{
		aC->HXC2auto = false;
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolR1.value));
		if (tmpIntU != aC->HXC2tolerance[0][1])
		{
			aC->HXC2tolerance[0][1] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolR2.isPassed == true)
	{
		aC->HXC2auto = false;
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolR2.value));
		if (tmpIntU != aC->HXC2tolerance[0][2])
		{
			aC->HXC2tolerance[0][2] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolR3.isPassed == true)
	{
		aC->HXC2auto = false;
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolR3.value));
		if (tmpIntU != aC->HXC2tolerance[0][3])
		{
			aC->HXC2tolerance[0][3] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolR4.isPassed == true)
	{
		aC->HXC2auto = false;
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolR4.value));
		if (tmpIntU != aC->HXC2tolerance[0][4])
		{
			aC->HXC2tolerance[0][4] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolR5.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolR5.value));
		if (tmpIntU != aC->HXC2tolerance[0][5])
		{
			aC->HXC2tolerance[0][5] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolG0.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolG0.value));
		if (tmpIntU != aC->HXC2tolerance[1][0])
		{
			aC->HXC2tolerance[1][0] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolG1.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolG1.value));
		if (tmpIntU != aC->HXC2tolerance[1][1])
		{
			aC->HXC2tolerance[1][1] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolG2.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolG2.value));
		if (tmpIntU != aC->HXC2tolerance[1][2])
		{
			aC->HXC2tolerance[1][2] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolG3.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolG3.value));
		if (tmpIntU != aC->HXC2tolerance[1][3])
		{
			aC->HXC2tolerance[1][3] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolG4.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolG4.value));
		if (tmpIntU != aC->HXC2tolerance[1][4])
		{
			aC->HXC2tolerance[1][4] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolG5.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolG5.value));
		if (tmpIntU != aC->HXC2tolerance[1][5])
		{
			aC->HXC2tolerance[1][5] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolB0.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolB0.value));
		if (tmpIntU != aC->HXC2tolerance[2][0])
		{
			aC->HXC2tolerance[2][0] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolB1.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolB1.value));
		if (tmpIntU != aC->HXC2tolerance[2][1])
		{
			aC->HXC2tolerance[2][1] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolB2.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolB2.value));
		if (tmpIntU != aC->HXC2tolerance[2][2])
		{
			aC->HXC2tolerance[2][2] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolB3.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolB3.value));
		if (tmpIntU != aC->HXC2tolerance[2][3])
		{
			aC->HXC2tolerance[2][3] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolB4.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolB4.value));
		if (tmpIntU != aC->HXC2tolerance[2][4])
		{
			aC->HXC2tolerance[2][4] = tmpIntU;				modified = 1;
		}
	}
	if (cli->HXC2tolB5.isPassed == true)
	{
		aC->HXC2auto = false; /* deactivates context_quant() */
		tmpIntU = atoi(unMarshal((String^)cli->HXC2tolB5.value));
		if (tmpIntU != aC->HXC2tolerance[2][5])
		{
			aC->HXC2tolerance[2][5] = tmpIntU;				modified = 1;
		}
	}
	/* SCF	*/
	if (cli->SCF_directional.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->SCF_directional.value));
		if (tmpIntU != aC->SCF_directional)
		{
			aC->SCF_directional = tmpIntU;				modified = 1;
		}
	}
	if (cli->SCF_prediction.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->SCF_prediction.value));
		if (tmpIntU != aC->SCF_prediction)
		{
			aC->SCF_prediction = tmpIntU;				modified = 1;
		}
	}
	if (cli->SCF_stage2tuning.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->SCF_stage2tuning.value));
		if (tmpIntU != aC->SCF_stage2tuning)
		{
			aC->SCF_stage2tuning = tmpIntU;				modified = 1;
		}
	}
	if (cli->SCF_colourMAP.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->SCF_colourMAP.value));
		if (tmpIntU != aC->SCF_colourMAP)
		{
			aC->SCF_colourMAP = tmpIntU;				modified = 1;
		}
	}
	if (cli->SCF_maxNumPatternsIdx.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->SCF_maxNumPatternsIdx.value));
		if (tmpIntU == 0) tmpIntU = 1;
		if (tmpIntU != aC->SCF_maxNumPatternsIdx)
		{
			aC->SCF_maxNumPatternsIdx = tmpIntU;				modified = 1;
		}
	}

	if (cli->CoBaLP2_sT.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->CoBaLP2_sT.value));

		if (tmpIntU != aC->CoBaLP2_sT)
		{
			aC->CoBaLP2_sT = (unsigned char)tmpIntU;
			modified = 1;
		}
	}
	if (cli->CoBaLP2_sA.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->CoBaLP2_sA.value));

		if (tmpIntU != aC->CoBaLP2_sA)
		{
			aC->CoBaLP2_sA = (unsigned char)tmpIntU;
			modified = 1;
		}
	}
	if (cli->CoBaLP2_sB.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->CoBaLP2_sB.value));

		if (tmpIntU != aC->CoBaLP2_sB)
		{
			aC->CoBaLP2_sB = (unsigned char)tmpIntU;
			modified = 1;
		}
	}
	if (cli->CoBaLP2_aR.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->CoBaLP2_aR.value));

		if (tmpIntU != aC->CoBaLP2_aR)
		{
			aC->CoBaLP2_aR = (unsigned char)tmpIntU;
			modified = 1;
		}
	}
	//if (cli->CoBaLP2_aL.isPassed == true)
	//{
	//	tmpIntU = atoi(unMarshal((String^)cli->CoBaLP2_aL.value));

	//	if (tmpIntU != aC->CoBaLP2_aL)
	//	{
	//		aC->CoBaLP2_aL = (unsigned char)tmpIntU;
	//		modified = 1;
	//	}
	//}
	if (cli->CoBaLP2_constLines.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->CoBaLP2_constLines.value));

		if (tmpIntU != aC->CoBaLP2_constLines)
		{
			aC->CoBaLP2_constLines = (unsigned char)tmpIntU;
			modified = 1;
		}
	}

	if (cli->predMode.isPassed == true)
	{
		if((String^)cli->predMode.value == "none")
			tmpPred = NONE;
		else if((String^)cli->predMode.value == "left")
			tmpPred = LEFTNEIGHBOUR;
		else if((String^)cli->predMode.value == "paeth")
			tmpPred = PAETH;
		else if((String^)cli->predMode.value == "med")
		{
			tmpPred = MED;
		}
		else if((String^)cli->predMode.value == "medpaeth")
		{
			tmpPred = MEDPAETH;
		}
		else if((String^)cli->predMode.value == "adapt")
		{
			tmpPred = ADAPT;
		}
		else if((String^)cli->predMode.value == "h26x")
		{
			tmpPred = H26x;
		}
		else if((String^)cli->predMode.value == "lmspred")
		{
			tmpPred = LMSPRED;
		}
		else if((String^)cli->predMode.value == "lsblend")
		{
			tmpPred = LSBLEND;
		}
		else if((String^)cli->predMode.value == "lspred")
		{
			tmpPred = LSPRED;
		}
		if (tmpPred != aC->predMode)
		{
			aC->predMode = tmpPred;
			modified = 1;
		}
		if (aC->coding_method != CoBaLP2 && aC->coding_method != LOCO)
		{
			if (aC->predMode == NONE)
			{
				aC->rct_type_used = aC->rct_type_nopr; /* chose better one	*/
			}
			else
			{
				aC->rct_type_used = aC->rct_type_pred; /* chose better one	*/
			}
		}
	}
	if (cli->LSnumOfPreds.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->LSnumOfPreds.value));
		aC->LSpred_numOfPreds = (unsigned char)tmpIntU;
	}
	if (cli->LSsearchSpace.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->LSsearchSpace.value));
		aC->LSpred_searchSpace = (unsigned char)tmpIntU;
	}
	if (cli->LSuseCorr_flag.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->LSuseCorr_flag.value));
		aC->useCorrelation_flag = tmpIntU ? 1:0;
	}
	if (cli->LSuseColPred_flag.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->LSuseColPred_flag.value));
		aC->useColPrediction_flag = tmpIntU ? 1:0;
	}
	if (cli->LSuseColParal_flag.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->LSuseColParal_flag.value));
		aC->useColPredictionP_flag = tmpIntU ? 1:0;
	}
	
	if (cli->indexed_flag.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->indexed_flag.value));

		if (tmpIntU != aC->indexed_flag)
		{
			aC->indexed_flag = (unsigned char)tmpIntU;
			modified = 1;
		}
	}

	if (cli->indexed_adj.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->indexed_adj.value));

		if (tmpIntU != aC->indexed_adj)
		{
			aC->indexed_adj = tmpIntU;
			modified = 1;
		}
	}

	if (cli->indexed_arith.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->indexed_arith.value));

		if (tmpIntU != aC->indexed_arith)
		{
			aC->indexed_arith = tmpIntU;
			modified = 1;
		}
	}

	if (cli->palette_sorting.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->palette_sorting.value));

		if (tmpIntU != aC->palette_sorting)
		{
			aC->palette_sorting = (unsigned char)tmpIntU;
			modified = 1;
		}
	}

	//if (cli->segmentWidth.isPassed == true)
	//{
	//	tmpIntU = atoi(unMarshal((String^)cli->segmentWidth.value));

	//	if (tmpIntU != aC->segmentWidth)
	//	{
	//		aC->segmentWidth = tmpIntU;
	//		modified = 1;
	//	}
	//}

	//if (cli->segmentHeight.isPassed == true)
	//{
	//	tmpIntU = atoi(unMarshal((String^)cli->segmentHeight.value));

	//	if (tmpIntU != aC->segmentHeight)
	//	{
	//		aC->segmentHeight = tmpIntU;
	//		modified = 1;
	//	}
	//}

	if (cli->interleaving_mode.isPassed == true)
	{
		if((String^)cli->interleaving_mode.value == "y_u_v")
			tmpInterleav = Y_U_V;
		else if((String^)cli->interleaving_mode.value == "yy_uvuv")
			tmpInterleav = YY_uvuv;
		else if((String^)cli->interleaving_mode.value == "yy_uuvv")
			tmpInterleav = YY_uuvv;
		else if((String^)cli->interleaving_mode.value == "yyuuvv")
			tmpInterleav = YYuuvv;
		else if((String^)cli->interleaving_mode.value == "yuvyuv")
			tmpInterleav = YuvYuv;

		if (tmpInterleav != aC->interleaving_mode)
		{
			aC->interleaving_mode = tmpInterleav;
			modified = 1;
		}
	}

	if (cli->skip_rlc1.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->skip_rlc1.value));

		if (tmpIntU != aC->skip_rlc1)
		{
			aC->skip_rlc1 = (unsigned char)tmpIntU;
			modified = 1;
		}
	}

	if (cli->skip_precoding.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->skip_precoding.value));

		if (tmpIntU != aC->skip_precoding)
		{
			aC->skip_precoding = (unsigned char)tmpIntU;
			modified = 1;
		}
	}

	if (cli->skip_postBWT.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->skip_postBWT.value));

		if (tmpIntU != aC->skip_postBWT)
		{
			aC->skip_postBWT = (unsigned char)tmpIntU;
			modified = 1;
		}
	}

	if (cli->postbwt.isPassed == true)
	{
		if((String^)cli->postbwt.value == "mtf")
		{
			tmpPostBWT = MTF;

			if (tmpPostBWT != aC->postbwt)
			{
				aC->postbwt = tmpPostBWT;
				modified = 1;
			}
		}
		else if((String^)cli->postbwt.value == "ifc")
		{
			tmpPostBWT = IFC;
			if (tmpPostBWT != aC->postbwt)
			{
				aC->postbwt = tmpPostBWT;
				modified = 1;
			}
		}
		else if((String^)cli->postbwt.value == "hxc3")
		{
			tmpPostBWT = HXC3;
			if (tmpPostBWT != aC->postbwt)
			{
				aC->postbwt = tmpPostBWT;
				modified = 1;
			}
		}
	}

	if (cli->IFCresetVal.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->IFCresetVal.value));
		if (tmpIntU != aC->IFCresetVal)
		{
			aC->IFCresetVal = tmpIntU;
			modified = 1;
		}
	}
	if (cli->IFCmaxCount.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->IFCmaxCount.value));
		if (tmpIntU != aC->IFCmaxCount)
		{
			aC->IFCmaxCount = tmpIntU;
			modified = 1;
		}
	}
	if (cli->MTF_val.isPassed == true)
	{
		tmpIntU = atoi(unMarshal((String^)cli->MTF_val.value));
		if (tmpIntU != aC->MTF_val)
		{
			aC->MTF_val = tmpIntU;
			modified = 1;
		}
	}

	if (cli->use_RLC2zero.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->use_RLC2zero.value));

		if (tmpCharU != aC->use_RLC2zero)
		{
			aC->use_RLC2zero = tmpCharU;
			modified = 1;
		}
	}

	if (cli->entropy_coding_method.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->entropy_coding_method.value));

		if (tmpCharU != aC->entropy_coding_method)
		{
			aC->entropy_coding_method = tmpCharU;
			modified = 1;
		}
	}

	if (cli->separate_coding_flag.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->separate_coding_flag.value));

		if (tmpCharU != aC->separate_coding_flag)
		{
			aC->separate_coding_flag = tmpCharU;
			modified = 1;
		}
	}

	return modified;
}

/*------------------------------------------------------------------
* cbool applyArgumentList(cliArguments* cli, H26xSettings* settings)
*
* applies the parameters provided via CLI to the H26xSettings struct
* Return Value: true if modifications were neccessary
*-----------------------------------------------------------------*/
cbool applyArgumentList(cliArguments* cli, H26xSettings* settings)
{
	cbool modified = 0;
	int i;
	unsigned char tmpCharU = 0;
	double tmpDouble = 0.0;

	// H.26x Part
	if (cli->mse_thresh.isPassed == true)
	{
		tmpDouble = atof(unMarshal((String^)cli->mse_thresh.value));
		if (tmpDouble != settings->config->mse_thresh)
		{
			settings->config->mse_thresh = tmpDouble;
			modified = 1;
		}
	}
	if (cli->logStatistics.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->logStatistics.value));

		if (tmpCharU != settings->config->logBlocks)
		{
			settings->config->logBlocks = tmpCharU;
			settings->config->logPredictors = tmpCharU;
			settings->config->logTransformations = tmpCharU;
			modified = 1;
		}
	}

	if (cli->printErrorPic.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->printErrorPic.value));

		if (tmpCharU != settings->config->printErrPic)
		{
			settings->config->printErrPic = tmpCharU;
			modified = 1;
		}
	}

	if (cli->h_e_predictors.isPassed == true)
	{
		Regex^ rx = gcnew Regex( "\\d+",static_cast<RegexOptions>(
								RegexOptions::Compiled | RegexOptions::IgnoreCase) );
		MatchCollection^ matches = rx->Matches( cli->h_e_predictors.value );

		// First: false all
		for(i = 0; i < NUM_INTRA_MODE; i ++)
		{
			settings->config->availablePredictors[i] = FALSE;
		}

		// Second: only enable given
		for each(Match^ match in matches)
		{
			settings->config->availablePredictors[atoi(unMarshal(match->Value))] = TRUE;
		}
		modified = 1;
	}

	if (cli->h_d_predictors.isPassed == true)
	{
		Regex^ rx = gcnew Regex( "\\d+",static_cast<RegexOptions>(
							RegexOptions::Compiled | RegexOptions::IgnoreCase) );
		MatchCollection^ matches = rx->Matches( cli->h_d_predictors.value );

		/* First: true all */
		for(i = 0; i < NUM_INTRA_MODE; i ++)
		{
			settings->config->availablePredictors[i] = TRUE;
		}

		/* Second: only disable given */
		for each(Match^ match in matches)
		{
			settings->config->availablePredictors[atoi(unMarshal(match->Value))] = FALSE;
		}

		modified = 1;
	}

	if (cli->blockSizes.isPassed == true)
	{
		Regex^ rx = gcnew Regex( "\\d+",static_cast<RegexOptions>(
							RegexOptions::Compiled | RegexOptions::IgnoreCase) );
		MatchCollection^ matches = rx->Matches( cli->blockSizes.value );

		/* First: false all */
		for(i = 0; i < NUM_BLOCK_SIZES; i ++)
		{
			settings->config->availableBlocks[i] = FALSE;
		}

		/* Second: only enable given */
		for each(Match^ match in matches)
		{
			i = getIdxForBlockSize(atoi(unMarshal(match->Value)));
			settings->config->availableBlocks[i] = TRUE;
		}
		modified = 1;
	}

	if (cli->postTransform.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->postTransform.value));

		if (tmpCharU != settings->config->postTransform)
		{
			settings->config->postTransform = tmpCharU;
			modified = 1;
		}
	}
	return modified;
}

/*------------------------------------------------------------------
* cbool applyArgumentList(cliArguments* cli, CustomParam* cparam)
*
* applies the parameters provided via CLI to the CustomParam struct
* Return Value: true if modifications were neccessary
*-----------------------------------------------------------------*/
cbool applyArgumentList(cliArguments* cli, CustomParam* cparam)
{
	cbool modified = 0;
	unsigned char tmpCharU;
	float tmpF = 0;
	int tmpInt;
	unsigned int i;

	// ADAPT Part
	if (cli->mode.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->mode.value));

		if (tmpCharU == 0)
		{
			// Mean Mode
			if (!cparam->predparam->wMeanMode)
			{
				cparam->predparam->wMeanMode = true;
				cparam->predparam->blockMode = false;
				modified = 1;
			}
		}
		else if(tmpCharU == 1)
		{
			// Block Mode
			if (!cparam->predparam->blockMode)
			{
				cparam->predparam->wMeanMode = false;
				cparam->predparam->blockMode = true;
				modified = 1;
			}
		}
	}

	if (cli->contextFile.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->contextFile.value));

		if (!cparam->usextfile)
		{
			cparam->usextfile = true;
			cparam->ctxfile = unMarshal((String^)cli->contextFile.value);
			modified = 1;
		}
	}

	if (cli->sigma.isPassed == true)
	{
		tmpF.TryParse(cli->sigma.value, tmpF);
		if (tmpF != cparam->predparam->tmSigma)
		{
			cparam->predparam->tmSigma= tmpF;
			modified = 1;
		}
	}

	if (cli->blockWidth.isPassed == true)
	{
		tmpInt = atoi(unMarshal((String^)cli->blockWidth.value));
		if (tmpInt > 255)
		{
			printf( "\n blockWidth must be smaller than 256 !\n");
			tmpCharU = 255;
		}
		else		tmpCharU = (unsigned char)tmpInt;

		if (tmpCharU != cparam->predparam->blockWidth)
		{
			cparam->predparam->blockWidth = tmpCharU;
			modified = 1;
		}
	}
	if (cli->blockHeight.isPassed == true)
	{
		tmpInt = atoi(unMarshal((String^)cli->blockHeight.value));
		if (tmpInt > 255)
		{
			printf( "\n blockHeight must be smaller than 256 !\n");
			tmpCharU = 255;
		}
		else		tmpCharU = (unsigned char)tmpInt;

		if (tmpCharU != cparam->predparam->blockHeight)
		{
			cparam->predparam->blockHeight = tmpCharU;
			modified = 1;
		}
	}
	if (cli->cxPx.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->cxPx.value));

		if (tmpCharU != cparam->predparam->cxPx)
		{
			cparam->predparam->cxPx = tmpCharU;
			modified = 1;
		}
	}
	if (cli->cxGrad.isPassed == true)
	{
		tmpCharU = (unsigned char)atoi(unMarshal((String^)cli->cxGrad.value));

		if (tmpCharU != cparam->predparam->cxGrad)
		{
			cparam->predparam->cxGrad = tmpCharU;
			modified = 1;
		}
	}

	if (cli->a_predictors.isPassed == true)
	{
		tmpInt = atoi(unMarshal((String^)cli->a_predictors.value));

		for (i = 0; i < 6; i++)
		{
			cparam->predparam->predictors[i] = (tmpInt >= 
											(pow(10.,(double)(cparam->predparam->anzPredictors - i - 1))));
			if (cparam->predparam->predictors[i] == TRUE)
			{
				tmpInt = tmpInt - 
									(int)(pow(10.,(double)(cparam->predparam->anzPredictors - i - 1)));
			}
		}
		modified = 1;
	}

	if (cli->a_bias.isPassed == true)
	{
		tmpInt = atoi(unMarshal((String^)cli->a_bias.value));

		for (i = 0; i < 6; i++)
		{
			cparam->predparam->bias[i] = (tmpInt >= 
											(pow(10.,(double)(cparam->predparam->anzPredictors - i))));
			if (cparam->predparam->bias[i] == TRUE)
			{
				tmpInt = tmpInt - 
									(int)(pow(10.,(double)(cparam->predparam->anzPredictors - i)));
			}
		}
		modified = 1;
	}
	return modified;
}

/*------------------------------------------------------------------
* void consoleOperation(SessionParameters* sP)
*
* performs the load and save sequence as requested in the 
* SessionParameters struct
*-----------------------------------------------------------------*/
void consoleOperation( SessionParameters* sP)
{
	char log_name[511];
	int coding_method_isPassed;
	String ^inputFileName, ^ouputFileName;
	String ^pureFilename;
	String ^ascii_name;
	String ^%asciiName = ascii_name;
	String ^ct_string;
	String^% rct_string=ct_string;	
	int modif_flag = 0;
#ifdef INTERNET_  /* defined in Form1.h */
	int log_flag = 0;	/* never print log info to file	*/
	log_flag = 1;	/* always print log info to file	*/
#else
	int log_flag = 1;	/* always print log info to file	*/
#endif
	int is_ASCII_flag;

	CustomParam cparam;
	AutoConfig autoConf, *aC;
	ImageFeatures imageF, *iF;
	INTERMEDIATE_VALS intermed_vals, *iv;
	BYTES_STRUC bytes_obj, *bytes;
	TimeSpan span;
	TimeSpan totalSpan;
	DateTime start, end;

	inputFileName  = Marshal::PtrToStringUni( (IntPtr)(wchar_t *)sP->inFN );
	ouputFileName = Marshal::PtrToStringUni( (IntPtr)(wchar_t *)sP->outFN);

	/* extract filename from path and convert to wide character string for output	*/
	pureFilename = Path::GetFileName( inputFileName);
	fwprintf( stderr, L"\n\n %s", (wchar_t*)(void*)Marshal::StringToHGlobalUni( pureFilename));
	fprintf( stderr, "\n loading image ...");
	
	/* ----------------------------------------------------------
	 * check filename
	 * if Unicode character(s), then create something usful
	 */
	is_ASCII_flag = check_filename( inputFileName, asciiName, 1);

	sP->pD = loadPicFile( inputFileName, nullptr, 1);


	if (sP->pD == NULL) return; /* image could not be loaded	*/

	bytes = &bytes_obj;

	aC = &autoConf;
	iF = &imageF;
	iv = &intermed_vals;


  /*----------------------
	 * from here, it should be in principal same as in Form1.cpp lines 115ff
	 */

	if (!ouputFileName->ToLower()->EndsWith(".tsip"))
	{
		savePicFile( ouputFileName, sP->pD, NULL,NULL,NULL,NULL,NULL, 1);
	}
	else
	{
		int i;
		double entropy[3]; /* entropies of colour components	*/
		unsigned char *usedColourSpaces= NULL;
		unsigned int numberOfTiles;
		unsigned int  xposFullPic, yposFullPic;
		PicData *tmpPD;
		FileInfo^ fout;
		//CompMode *usedMethods = NULL;
		int *usedMethods = NULL;
		AutoConfig original_aC;
		ImageFeatures original_iF;

		/* pass this info to next processing steps, as some of them could be skipped	*/
		if (sP->cli.coding_method.isPassed == true)
		{
				coding_method_isPassed = 1;
				/* only aC->coding_method would be required here to be set	*/
				/* No!!!! this is problematic, because the automatic settings wont be activated!!! */
				applyArgumentList(&(sP->cli), aC); 
		}
		else
		{
			 coding_method_isPassed = 0;
			 modif_flag = 0;
		}

		fprintf( stderr, "\n  determination of image features ...");
		/* get image features and the automatic settings of the entire image	*/
		tsipWrapper_1( sP->pD, iF, aC, entropy, coding_method_isPassed);
		//modif_flag = false; /* automatic settings only	*/
		/* Update the configuration and check for changes
		 * modif_flag might already be set to true	
		 */
		aC->locoImproveFlag = 0; /* default in batch mode!	*/
		modif_flag |= applyArgumentList(&(sP->cli), aC); 
		/* here as sP->cli.rct_type.value is evaluated below and 
		 * rct.value must be mapped to number beforehand
		 */

		/* determine tile size if parameters are not passed	*/
		if ( !sP->cli.tileWidth.isPassed && !sP->cli.tileHeight.isPassed)
		{
			getTileSize( sP->pD, &(aC->tileWidth), &(aC->tileHeight),
				&(aC->maxTileWidth), &(aC->maxTileHeight), iF->numColours);
		}

		/* keep the automatic settings of the entire image	*/
		original_aC = *aC;
		original_iF = *iF;

		start = DateTime::Now;

		/* parse for tile size, all other options are parsed again for each tile
		 * overwriting the automatic settings
		 */
		if ( sP->cli.tileWidth.isPassed)
		{
			unsigned int tileWidth;
			tileWidth = atoi(unMarshal((String^)sP->cli.tileWidth.value));
			if (tileWidth < max( sP->pD->height, sP->pD->width) && tileWidth < 64)
			{
				fprintf( stderr, "\n chosen tile size (%d) is too small!", tileWidth);
				fprintf( stderr, "\n Tile size is set to 64!\n");
				tileWidth = 64;
			}
			aC->tileWidth = tileWidth;
			//aC->tileHeight = tileWidth; /* no distinction yet	*/
		}
		if ( sP->cli.tileHeight.isPassed)
		{
			unsigned int tileHeight;
			tileHeight = atoi(unMarshal((String^)sP->cli.tileHeight.value));
			if (tileHeight < sP->pD->height && tileHeight < 64)
			{
				fprintf( stderr, "\n chosen tile height (%d) is too small!", tileHeight);
				fprintf( stderr, "\n Tile height is set to 64!\n");
				tileHeight = 64;
			}
			aC->tileHeight = tileHeight; /* no distinction yet	*/
		}
		// use automatic settings //else tileWidth = max( sP->pD->height, sP->pD->width);

		numberOfTiles = getNumberOfTiles( sP->pD, aC->tileWidth, aC->tileHeight);
		if ( sP->cli.HXC2auto.isPassed)
		{
			unsigned int tmpIntU;
			tmpIntU = atoi(unMarshal((String^)sP->cli.HXC2auto.value));

			if (tmpIntU != aC->HXC2auto)
			{
				aC->HXC2auto = tmpIntU;
				modif_flag = 1;
			}
		}

		if ( sP->cli.rct_type.isPassed)
		{
			unsigned int tmpIntU;
			tmpIntU = atoi(unMarshal((String^)sP->cli.rct_type.value));

			if (tmpIntU != aC->rct_type_used)
			{
				aC->rct_type_used = (unsigned char)tmpIntU;
				aC->rct_type_nopr = (unsigned char)tmpIntU;
				aC->rct_type_pred = (unsigned char)tmpIntU;
				modif_flag = 1;
			}
		}
		else if (aC->coding_method == HXC2)
		{	/* use RCT w/o prediction	*/
			aC->rct_type_used = aC->rct_type_nopr;
		}
		else if (aC->coding_method == SCF)
		{	/* use RCT w/o prediction	*/
			aC->rct_type_used = aC->rct_type_nopr;
			if ( sP->cli.histMode.isPassed == false)
			{
				aC->histMode = COMPACTION;
			}
			aC->SCF_maxNumPatterns = mapIdx2maxNumPatterns( aC->SCF_maxNumPatternsIdx);
		}


		/* write global header and close file afterwards */
		{
			wchar_t *outFileName;
			outFileName = (wchar_t*)(void*)Marshal::StringToHGlobalUni( ouputFileName);

			writeGlobalHeader( sP->pD, numberOfTiles, outFileName);	

			Marshal::FreeHGlobal( IntPtr( outFileName));
		}
		bytes->num_globheader_bytes = numBytes; /* declared in bitbuf.c	*/
		//bytes->num_globheader_bytes = writeGlobalHeaderToFile( sP->pD, 
		//							numberOfTiles, ouputFileName);	

		if (numberOfTiles > 1)
		{
			//ALLOC( usedMethods, numberOfTiles, CompMode);
			ALLOC( usedMethods, numberOfTiles, int);
			ALLOC( usedColourSpaces, numberOfTiles, unsigned char);
		}
		// Geändert Team Darmstadt
		//tmpPD = CopyPicData(sP->pD); /* copy entire image	*/
		tmpPD = sP->pD; /* save pointer to image	*/
		/*Momentane Position der X und Y Zeiger, geben an wo die nächste Kachel aus dem Bild geschnitten werden soll*/
		xposFullPic = 0;
		yposFullPic = 0;
		for ( i = 0; i < (signed)numberOfTiles; i++)
		{
			if (numberOfTiles > 1)
			{ /* remember tile number	*/
				sP->pD = getTile( tmpPD, aC->tileWidth, aC->tileHeight, &xposFullPic, &yposFullPic, i);
				fprintf( stderr, " (total: %d tiles)", numberOfTiles); 
				sP->pD->tileNum = (unsigned int)(i+1);

				/* compute some statistics for automatic parameter selection	*/
				fprintf( stderr, "\n  determination of image features ...");

				/* and automatic decision making for compression parameters	*/

				/* this is also called  in Form1.cpp in xx()	*/
				tsipWrapper_1( sP->pD, iF, aC, entropy, coding_method_isPassed); /* includes auto setting of parameters	*/

				/* Update the configuration and check for changes*/
				modif_flag = applyArgumentList(&(sP->cli), aC);

				/* correct auto settings, if parameter was passed to CLI	*/
				if ( sP->cli.HXC2auto.isPassed)
				{
					unsigned int tmpIntU;
					tmpIntU = atoi(unMarshal((String^)sP->cli.HXC2auto.value));

					if (tmpIntU != aC->HXC2auto)
					{
						aC->HXC2auto = tmpIntU;
						modif_flag = 1;
					}
				}

				if ( sP->cli.rct_type.isPassed)
				{
					unsigned int tmpIntU;
					tmpIntU = atoi(unMarshal((String^)sP->cli.rct_type.value));

					if (tmpIntU != aC->rct_type_used)
					{
						aC->rct_type_used = (unsigned char)tmpIntU;
						modif_flag = 1;
					}
				}
				else if (aC->coding_method == HXC2 || aC->coding_method == SCF)
				{	/* use RCT w/o prediction	*/
					aC->rct_type_used = aC->rct_type_nopr;
				}
			}
			else /* do not recompute settings, when a single tile	*/
			{
				sP->pD->tileNum = 0;	/* marker for single segment (is default)	*/
			}


			if ( i == 0)
			{
				/* copy the logfile name only the first time	*/
				if (aC->log_name == NULL) log_flag = 0; /*strcpy( log_name, "log_TSIP.txt");*/
				else strcpy( log_name, aC->log_name);
			}


			/* prediction parameter	*/
			/* applyArgumentList( cli, cparam) is called inside	*/
			if ( 0 == tsipWrapper_2( sP->pD, iF, aC, &cparam, &(sP->cli), 0, &modif_flag) )
				return; /* should not happen in console mode	*/

			if (aC->predMode == H26x)
			{
				ALLOC(aC->H26x_Set,1u,H26xSettings);
				ALLOC(aC->H26x_Set->config,1u,H26xConfiguration);

				// default config
				getH26xDefaultConfig(aC->H26x_Set->config);

				// MST: Update new config
				modif_flag |= applyArgumentList(&(sP->cli), aC->H26x_Set);
			}

			fprintf( stderr, "\n saving image ...          ");


			/* call encoding routine, also called in CLI.cpp	*/
			span = tsipWrapper_3( ouputFileName, inputFileName, sP->pD, aC, iF, iv, &cparam, bytes, 
 				modif_flag, log_flag, log_name, rct_string, 1 /* console_flag*/);

			if (numberOfTiles > 1)
			{
				usedMethods[i] = aC->coding_method;
				usedColourSpaces[i] = aC->rct_type_used;
			}

			/* Parameter löschen */
			if (aC->predMode == ADAPT)
			{
				freeParameter( cparam.predparam);
			}

			if (aC->predMode == H26x)
			{
				FREE(aC->H26x_Set->config);
				FREE(aC->H26x_Set);
			}

			if (numberOfTiles > 1)
			{
				DeletePicData( sP->pD);
			}

		} /* for ( i = 0; i < numberOfTiles;	*/
		sP->pD = tmpPD; /* set back to original pointer	*/

		end = DateTime::Now;
		totalSpan = end - start;

		/* ---------------------------------------
		* do the logging of entire image
		*/
		fout = gcnew FileInfo( ouputFileName);
		if (log_flag && numberOfTiles > 1)
		{
			char *Cfilename = NULL;
			int is_ASCII_flagL;
			String ^ascii_nameL;
			String ^%asciiNameL = ascii_nameL;

			fprintf( stderr, "\n log for entire image ...");

			/* ----------------------------------------------------------
			 * check filename
			 * if Unicode character(s), then create something usful
			 */
			is_ASCII_flagL = check_filename( inputFileName, asciiNameL, 0);
			Cfilename = (char *)(void *)Marshal::StringToHGlobalAnsi( asciiNameL);

			// Function takes way to much arguments -> stack cluttering, consider refactoring

			/* check, whether all tiles are coded with the same method	*/
			original_aC.coding_method = usedMethods[0];
			for ( i = 1; i < (signed)numberOfTiles; i++)
			{
				if (usedMethods[i] != usedMethods[0])
				{
					original_aC.coding_method = MiXeD;
					break;
				}
			}
			/* check, whether all tiles are coded with the same colour space	*/
			original_aC.rct_type_used = usedColourSpaces[0];

			/* get RCT string from number of colour space	*/
			// ct_string = rct2string( &original_aC);
			ct_string = rct2string( original_aC.rct_type_used );

			for ( i = 1; i < (signed)numberOfTiles; i++)
			{
				if (usedColourSpaces[i] != usedColourSpaces[0])
				{
					ct_string = "mixed  ";
					break;
				}
			}

			logResultsToFile( log_name, totalSpan, ct_string, modif_flag, fout, 
				Cfilename, sP->pD, &original_aC, &original_iF, iv, &cparam, bytes,
				1);

			Marshal::FreeHGlobal( IntPtr( Cfilename));
		}

		{
			double bpp;

			bpp = 8.0 * (unsigned long)fout->Length / (sP->pD->size);
			fprintf( stdout, "\n compressed to %ld bytes, %7.4lf bpp\n", (unsigned long)fout->Length, bpp);
		}
		Marshal::FreeHGlobal( IntPtr( aC->log_name)); /* set in setsessionparameters()	*/
		if (numberOfTiles > 1)
		{
			FREE( usedMethods);
			FREE( usedColourSpaces);
		}
	}	/* if (outFN->ToLower()->EndsWith(".tsip"))*/
}