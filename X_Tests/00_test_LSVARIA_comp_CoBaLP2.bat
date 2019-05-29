@echo off

setlocal enabledelayedexpansion

set EXE="..\bin_debug\TSIPcoder_2_16_.exe"
set EXE="..\bin\TSIPcoder_2_16_.exe"
set PSNR="PSNR_C.exe"
set logfile="log_LSVari_comp.txt"
set OutFile=tmpLSVaricomp.tsip
set res="res_LSVaricomp.txt"
set res_de="res_LSVaricomp_de.txt"
set DIR="..\..\..\Daten\CT2\Grey"
set DIR=..\..\..\Texte\Paper\2015-CoBaLP2_colour\Investigations\Testimages_YUV

REM -method TSIP  -pred ADAPT -skip_rlc1 0 -RLC2_zeros 0 -entropy_coding 0 -predictors 111111 -blockWidth 100 -blockHeight 100 -o tmp.tsip -i C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\Farbkreis_Itten_1961.ppm 

REM "Supported options: \n");
REM " -help 1 \n");
REM  -i input_file (*.tsip, *.png, *.bmp, *.ppm, *pgm, ...)
REM  -o output_file (*.tsip, *.png, *.bmp, *.ppm, *pgm, ...) 
REM  -rct ( 0..119; colour space ) 
REM  -Idx (1 ... use colour palette) 
REM  -Idx_sorting (0 ... Idx_GRB; 1 ... Idx_Lum) 
REM  -method (BPC, TSIP, LOCO, LOCO_T4, CoBaLP2, HXC) 
REM  -histMode (comp, none, perm) 
REM  -pred (none, LEFT, PAETH, MED, MEDPAETH, ADAPT, H26x) 
REM ## BPC options ##  
REM  -BPCauto (1 ... manual BPC settings will be ignored) 
REM  -tol (0...1; tolerance for BPC coding) 
REM  -tolOff1 (0...10; tolerance offset1 ) 
REM  -tolOff2 (0...20; tolerance offset2 ) 
REM  -BPCincreasefac (0...100; increase factor) 
REM ## CoBaLP2 options ##  
REM  -sT 1 (skip template matching) 
REM  -sA 1 (skip predictor A) 
REM  -sB 1 (skip predictor B) 
REM  -aR 1 (enable predictor R) 
REM ## TSIP options ##  
REM  -segWidth  \n");
REM  -segHeight \n");
REM  -interleaving (Y_U_V, YY_uuvv, YY_uvuv, YuvYuv, YYuuvv) 
REM  -skip_rlc1    (0 ... do not skip; 1 ... skip first run-length coding) 
REM  -skip_prec    (0 ... do not skip; 1 ... skip entire pre-coding stage) 
REM  -skip_postBWT (0 ... do not skip; 1 ... skip MTF/IFC) 
REM  -postBWT (MTF, IFC) 
REM  -MTF_val (0 ... 100) 
REM  -IFCmax (1 ... 100) 
REM  -IFCreset (1 ... 10000) 
REM  -RLC2_zeros (1 ... combine zeros only) 
REM  -entropy_coding (0... Huffman; 1 ... arithmetic) 
REM  -separate (0... jointly coding; 1 ... separate coding 
REM ADAPT prediction: 
REM  -blockWidth  (5 - 128) 
REM  -blockHeight (5 - 128) 
REM  -predictors (100000 ... use first predictor) 
REM              (010000 ... use second predictor) 
REM              (110000 ... use two first predictors) 
REM              (101000 ... use first and third predictor) 
REM              (011000 ... use second and third predictor) 
REM              (011111 ... use all predictors but first) 
REM 


REM check all images from the beginning
set flag=1

REM echo #Results > %res%

REM ##################### Main Loop ##############################
REM		 	%DIR%\barbara.y.pgm %DIR%\kodim04.pgm	%DIR%\Ashton_landscape_grey.pgm %DIR%\squirrel.png 
for  %%I in (    %DIR%\*_Y.pgm %DIR%\RNAi_dna_12bit.pgm
)do (

 echo %%I
REM  echo .
REM  echo   %%I   >> %res%



REM start with this images
 if %%I == ..\..\..\Texte\Paper\2015-CoBaLP2_colour\Investigations\Testimages_YUV\cafe.rgb_b01_Y.pgm (
 	set flag=1
  echo !flag!
 )

 if !flag! == 1 (

 echo %%I
 echo .
 echo   %%I   >> %res%

REM	for /L %%S in (5,2,17) do (
REM		for /L %%P in (6,6,18) do (
		%EXE% -i %%I  -o %OutFile% -searchSpace 19 -numOfPreds 18 -method TSIP -pred lsblend -corr 0 -tilewidth 9999999 -tileheight 999999 -log_name %logfile% >> %res%
REM			)
REM	)
REM %EXE% -i %OutFile% -o reco.pgm  >> %res_de%
REM %PSNR% -i1 %%I -i2 reco.pgm  -p  >> %res%

REM  del recoA.pgm
 )

REM ##### end of main loop #########
)



:ende




pause

REM@echo on