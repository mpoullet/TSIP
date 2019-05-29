@echo off

setlocal enabledelayedexpansion


REM 
set EXE="..\bin\TSIPcoder_3.exe"
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


set res_en="res_test_tmp.txt"

REM 
echo #Results >> %res_en%
set flag=1


REM ##################### Main Loop ##############################
for  %%I in (
       C:\Users\Strutz\Documents\Daten\CT2\Science\Map_118230550922074.ppm
     "C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\Sta01AUS(vertical).ppm"
        C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\PATNIXONandQEII-png.ppm
       C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\Bermet..ppm
       "C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\Rac01AUS(vertical).ppm"
       C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\ImperiumRevelation2.0.ppm
       C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\C.T..png
       C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\History_Hungary_1.ppm
      C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\Blanco1_0.png
     C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\Android-2.3.ppm
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\01-03_rouders.ppm
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\aspens.ppm
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\WEBDESIGNER_ISSUE_100.png
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\cov-journal-science-prev2-med.png
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\png-IceAlpha-ie5mac.ppm
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\png-8passes_PNGOUT.ppm
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\kbm-oslocentralxmas-34t.png
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\BML_CAT_Chartres_03k_PNGOUT.ppm
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\FRD-Bonn-20s-882x623.ppm
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\ecb_berlin_07t.ppm
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\ecb_ny_89t.png
       C:\Users\Strutz\Documents\Daten\CT2\Test_Set\png-9passes.ppm
	C:\Users\Strutz\Documents\Daten\CT2\PWC-Corpus\fractal.ppm
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

 %EXE% -i "%%I" -o tmpM2.tsip -log_name log_TSIP.txt >> %res_en% 
REM %EXE% -i tmpM.tsip -o recoM.ppm
REM %PSNR% -i1 "%%I" -i2 recoM.ppm  -p  >> %res_en%



REM del recoM.ppm
 
 )

REM ##### end of main loop #########
)

pause

REM
@echo on