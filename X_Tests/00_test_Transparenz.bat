REM
@echo off

REM ist f�r if flag Abfrage erforderlich
REM setlocal enabledelayedexpansion


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




set res_en="res_test_Transparenz.txt"
set res_de="res_test_Transparenz_d.txt"

REM 
echo #Results >> %res_en%
echo #Results >> %res_de%
set flag=1


REM ##################### Main Loop ##############################
for  %%I in (                               
      C:\Users\Strutz\Documents\Daten\Transparenz\*.png       
) do (

echo "%%I"
set "file=%%I"

echo %file%


REM start with this images
REM  if "%%I" == "C:\Users\Strutz\Documents\Daten\CT2\Science\b001793kf7.png" (
REM  	set flag=1
REM   echo !flag!
REM  )

REM pause

REM  if !flag! == 1 (


 echo .
 echo   "%%I"   >> %res_en%
 echo   "%%I"   >> %res_de%




    %EXE% -i "%%I" -o tmpT.tsip  -log_name log_TSIP_Transparenz.txt >> %res_en% 
REM  
   %EXE% -i tmpT.tsip -o recoT.png  >> %res_de%
REM 
    %PSNR% -i1 "%%I" -i2 recoT.png  -p  >> %res_en%

REM   
pause
  del recoT.png
 
REM  )

REM ##### end of main loop #########
)

pause

REM@echo on