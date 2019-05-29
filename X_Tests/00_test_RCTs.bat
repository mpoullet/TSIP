@echo off

REM 
set EXE="..\bin\TSIPcoder_1_39a_rev.exe"
set PSNR="PSNR_C.exe"
set outFile="tmpRCT.tsip"
set recoFile="recoRCT.ppm"
set LogFile=log_TSIP_RCTs.txt

REM -method TSIP  -pred ADAPT -skip_rlc1 0 -RLC2_zeros 0 -entropy_coding 0 -predictors 111111 -blockWidth 100 -blockHeight 100 -o tmp.tsip -i C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\Farbkreis_Itten_1961.ppm 

REM "Supported options: \n");
REM " -help 1 \n");
REM " -rct (colour space, 0...119) \n");
REM " -method (TSIP, LOCO, LOCO_T4, BE, HXC, CoBaLP2) \n");
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

set res="res_RCTs.txt"

REM 
echo #Results > %res%



for  %%I in (   
C:\Users\Strutz\Documents\Daten\CT2\Science\408px-Killersudoku_color.png

) do (

echo %%I
 echo .
 echo   %%I   >> %res%

REM for /L {Variable} IN (Startzahl, Schrittweite, Endzahl) DO (Aktion)
for /L %%C in (110, 1, 118) do (

 echo RCT:%%C .


REM 
 %EXE% -i %%I -o %outFile% -method LOCO -rct %%C -log_name %LogFile%>> %res%


REM  
%EXE% -i %outFile% -o %recoFile% >> res_de.txt
REM 
 %PSNR% -i1 %%I -i2 %recoFile%  -p  >> %res%


REM erase reco.ppm
 
 )
)

pause

REM@echo on