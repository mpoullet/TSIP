@echo off

setlocal enabledelayedexpansion


REM 
set EXE="..\bin\TSIPcoder_1.exe"
REM set EXE="..\bin_debug\TSIPcoder.exe"
set PSNR="PSNR_C.exe"

REM "Supported options: \n");
REM " -help 1 \n");
REM  -i input_file (*.tsip, *.png, *.bmp, *.ppm, *pgm, ...)
REM  -o output_file (*.tsip, *.png, *.bmp, *.ppm, *pgm, ...) 
REM  -rct ( 0..119; colour space ) 
REM  -Idx (1 ... use colour palette) 
REM  -Idx_sorting (0 ... Idx_GRB; 1 ... Idx_Lum) 
REM  -method (HXC, TSIP, LOCO, LOCO_T4, CoBaLP2) 
REM  -histMode (comp, none, perm) 
REM  -pred (none, LEFT, PAETH, MED, MEDPAETH, ADAPT, H26x) 
REM ## HXC options ##  
REM  -HXCauto (1 ... manual HXC settings will be ignored) 
REM  -tol (0...1; tolerance for HXC coding) 
REM  -tolOff1 (0...10; tolerance offset1 ) 
REM  -tolOff2 (0...20; tolerance offset2 ) 
REM  -HXCincreasefac (0...100; increase factor) 
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



set res_en="res_test_Maps_PWC.txt"

REM 
echo #Results >> %res_en%
set flag=1


REM ##################### Main Loop ##############################
for  %%I in (
       C:\Users\Strutz\Documents\Daten\CT2\Maps\*.png
       C:\Users\Strutz\Documents\Daten\CT2\PWC-Corpus\*.ppm
) do (

echo "%%I"

REM start with this images
 if "%%I" == "C:\Users\Strutz\Documents\Daten\CT2\Maps\T1_01_c201401.png" (
 	set flag=1
  echo !flag!
 )

 if !flag! == 1 (


 echo .
 echo   "%%I"   >> %res_en%


REM  %EXE% -i "%%I" -o tmp1.tsip -method CoBaLP2 -histMode comp -Idx 0  >> %res_en%
REM %EXE% -i %%I -o tmp1.tsip -method BPC  -histMode none -Idx 0  1>> %res_en% 2>>&1

 %EXE% -i "%%I" -o tmpM.tsip -method HXC2 -HXC2auto 1 -log_name log_TSIP_Maps_PWC.txt >> %res_en% 
REM %EXE% -i tmpM.tsip -o recoM.ppm
REM %PSNR% -i1 "%%I" -i2 recoM.ppm  -p  >> %res_en%



REM del recoM.ppm
 
 )

REM ##### end of main loop #########
)

pause

REM
@echo on