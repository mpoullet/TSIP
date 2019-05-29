REM
 @echo off

setlocal enabledelayedexpansion


REM 
set EXE="C:\Users\Strutz\Documents\C\TSIPcoder_1_33\bin\TSIPcoder_1_33_.exe"
REM set EXE="..\bin_debug\TSIPcoder.exe"
set PSNR="PSNR_C.exe"

set OutFile=tmpbTS2.tsip
set LogFile=log_TSIP_Best_Set2.txt

REM dir   \B    C:\Users\Strutz\Documents\Daten\CT2\Test_Set\*.ppm     > filenames.txt 
REM dir   \B   C:\Users\Strutz\Documents\Daten\CT2\Test_Set\*.png        >> filenames.txt    
REM dir   \B   C:\Users\Strutz\Documents\Daten\CT2\Test_Set\Not-HXC\*.png        >> filenames.txt    
REM dir   \B   C:\Users\Strutz\Documents\Daten\CT2\Test_Set\Not-HXC\*.ppm        >> filenames.txt    


REM "Supported options: \n");
REM " -help 1 \n");
REM  -i input_file (*.tsip, *.png, *.bmp, *.ppm, *pgm, ...)
REM  -o output_file (*.tsip, *.png, *.bmp, *.ppm, *pgm, ...) 
REM  -rct ( 0..119; colour space ) 
REM  -Idx (1 ... use colour palette) 
REM  -Idx_sorting (0 ... Idx_GRB; 1 ... Idx_Lum) 
REM  -method (HXC, TSIP, LOCO, LOCO_T4, CoBaLP2) 
REM  -histMode (comp, none, perm, kopt) 
REM  -pred (none, LEFT, PAETH, MED, MEDPAETH, ADAPT, H26x) 
REM  -tilesize 
REM ## HXC options ##  
REM  -HXCauto (1 ... manual HXC settings will be ignored) 
REM  -tol (0...1; tolerance for HXC coding) 
REM  -tolOff1 (0...10; tolerance offset1 ) 
REM  -tolOff2 (0...20; tolerance offset2 ) 
REM  -HXCincreasefac (0...100; increase factor) 
REM  ## HXC2 options ##  \n");
REM  -HXC2auto (1 ... manual HXC2 settings will be ignored) \n");
REM  -HXC2increasefac (0...100; increase factor) \n");
REM  -tolR0  \n");
REM  -tolR1  \n");
REM  -tolR2  \n");
REM  -tolR3  \n");
REM  -tolR4  \n");
REM  -tolR5  \n");
REM  -tolG0  \n");
REM  -tolG1  \n");
REM  -tolG2  \n");
REM  -tolG3  \n");
REM  -tolG4  \n");
REM  -tolG5  \n");
REM  -tolB0  \n");
REM  -tolB1  \n");
REM  -tolB2  \n");
REM  -tolB3  \n");
REM  -tolB4  \n");
REM  -tolB5  \n");
REM ## CoBaLP2 options ##  
REM  -sT 1 (skip template matching) 
REM  -sA 1 (skip predictor A) 
REM  -sB 1 (skip predictor B) 
REM  -aR 1 (enable predictor R) 
REM  -constLines 0 (disable constant lines) \n");
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




set res_en="res_test_Test_Set2.txt"
set flag=0

REM capture the current directory
set "currentDir=%cd%"

echo %currentDir%

REM change to directory with images
cd C:\Users\Strutz\Documents\Daten\CT2\Test_Set\Not-HXC
REM cd C:\Users\Strutz\Documents\Daten\CT2\Test_Set


if !flag! == 1 (
%EXE% -i DarkWorld.ppm  -o tmpB.tsip -method tsip -idx 1 -idx_sorting 0 -skip_rlc1 1 -IFCreset 8 -IFCmax 3 -log_name log_TSIP_Best_Set.txt >> %res_en%

)

%EXE% -i Bretagne2.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%


if !flag! == 1 (



echo end of file list

)
 


REM go back to initial directory
cd %currentDir%


pause

REM@echo on