REM 
@echo off

REM damit Zeit aktuakisiert wird:
SetLocal EnableDelayedExpansion


REM 
set EXE=".\TSIPcoder_2_13.exe"
set PSNR="..\X_Tests\PSNR_C.exe"

set OutFile=tmpAuto.tsip
set Recofile="reco.ppm"
set LogFile=log_TSIP.txt
set res="res_en.txt"


REM "Supported options: \n");
REM " -help 1 \n");
REM " -rct (colour space, 0...119) \n");
REM " -method (TSIP, LOCO, LOCO_T4, BPC, HXC, CoBaLP2) \n");
REM " -histMode(comp, none, perm) \n");
REM " -tol (0...200; tolerance for HXC coding) \n");
REM " -tolOff1 (tolerance offset1 ) \n");
REM " -tolOff2 (tolerance offset12 ) \n");
REM " -HXCincreasefac (increase factor) \n");
REM " -Idx (1 ... use colour palette) \n");
REM " -Idx_sorting (0 ... Idx_GRB; 1 ... Idx_Lum) \n");
REM " -Idx_adj (1, requires Idx) \n");
REM " -Idx_arith (1, requires Idx_adj) \n");
REM " -segWidth () \n");
REM " -segHeight () \n");
REM " -interleaving (Y_U_V, YY_uuvv, YY_uvuv, YuvYuv, YYuuvv) \n");
REM " -pred (none, LEFT, PAETH, MED, MEDPAETH, ADAPT, H26x) \n");
REM " -skip_rlc1    (0 ... do not skip; 1 ... skip first run-length coding) \n");
REM " -skip_prec    (0 ... do not skip; 1 ... skip entire pre-coding stage) \n");
REM " -skip_postBWT (0 ... do not skip; 1 ... skip MTF/IFC) \n");
REM " -postBWT (MTF, IFC) \n");
REM " -MTF_val (0 ... 100) \n");
REM " -IFCmax (1 ... 100) \n");
REM " -IFCreset (1 ... 10000) \n");
REM " -RLC2_zeros (1 ... combine zeros only) \n");
REM " -entropy_coding (0... Huffman; 1 ... arithmetic) \n");
REM " -separate (0... jointly coding; 1 ... separate coding \n");
REM "H26x prediction: \n");
REM " -mse_thresh \n");
REM " -errorPicture \n");
REM " -logStatistics (1... enable logging)\n");
REM " -errorPicture (1... enable output)\n");
REM " -enablePredictors (?)\n");
REM " -disablePredictors \n");
REM " -blockSizes \n");
REM " -postTransform (1 ... enable transformation after prediction)\n");
REM "ADAPT prediction: \n");
REM " -blockWidth  (5 - 128) \n");
REM " -blockHeight (5 - 128) \n");
REM " -predictors (100000 ... use first predictor) \n");
REM "             (010000 ... use second predictor) \n");
REM "             (110000 ... use two first predictors) \n");
REM "             (101000 ... use first and third predictor) \n");
REM "             (011000 ... use second and third predictor) \n");
REM "             (011111 ... use all predictors but first) \n");
REM " -bias (?)\n");
REM " -sigma (for TM)\n");



REM 
echo #Results > %res%

REM 
for  %%I in (
"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\CoBaLP2\1_Earth_(beschriftet).ppm"
) do (


set HIST=-histMode comp
REM 
set HIST=-histMode nomod

set TOLR=-tolR0 2 -tolR1 2 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1
set TOLG=-tolG0 2 -tolG1 2 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1
set TOLB=-tolB0 2 -tolB1 2 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1


set TSIP=-Idx 1 -idx_sorting 1 -skip_rlc1 1 -postBWT IFC -IFCmax 4 -IFCreset 15
set HXC2a=-HXC2auto 1 -method HXC2 -rct 1
set HXC2=-method HXC2 -rct 1 
set BPC=-method BPC -Idx 1 -idx_sorting 0
set HXC=-method HXC -Idx 1  -idx_sorting 0
set PARAM=-method SCC -HXC2auto 1 -histmode comp

set PARAM=-tilewidth 250 -tileheight 200

REM %EXE% -i %%I -o tmpt.tsip !HXC! !HIST!   >> %res%

REM %EXE% -i %%I -o tmpt.tsip  !HIST!  !HXC! -tilesize 5000


REM %EXE% -i %%I -o %Outfile%  !PARAM!   -log_name %Logfile% >> %res% 
%EXE% -i %%I -o %Outfile%  -st 0  -log_name %Logfile% >> %res% 


REM  %EXE% -i tmpt.tsip -o reco.ppm  >> res_de.txt
REM  %PSNR% -i1 %%I  -i2 reco.ppm  -p  >> %res%

 
 

REM  erase reco.ppm
 
)
pause

REM@echo on