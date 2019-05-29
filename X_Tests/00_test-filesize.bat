@echo off

setlocal enabledelayedexpansion

REM 
set EXE="..\bin\TSIPcoder.exe"
REM set EXE="..\bin_debug\TSIPcoder.exe"
set PSNR="PSNR_C.exe"

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

REM -o  tmp.tsip -method TSIP -Idx 1 -method BPC -i C:\Users\Strutz\Documents\Daten\science\5colors.png  
REM -o  tmp.tsip -method TSIP -Idx 0 -rct 1 -method BPC -i C:\Users\Strutz\Documents\Daten\science\5colors.png  

REM for  %%I in (C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\5colors.png ) do (
REM for  %%I in (C:\Users\Strutz\Documents\Daten\science\apple-world-aquagraph-16074.ppm C:\Users\Strutz\Documents\Daten\science\408px-Killersudoku_color.ppm) do (
REM for  %%I in (C:\Users\Strutz\Documents\Daten\science\5colors.ppm ) do (
REM for  %%I in (C:\Users\Strutz\Documents\Daten\science\ScienceWeek2010_03.png 
REM  C:\Users\Strutz\Documents\Daten\science\TNCCA.ppm 
REM  C:\Users\Strutz\Documents\Daten\science\ss433_natural_colour.ppm 
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\europa_karte_de_PNGOUT.ppm 
REM  C:\Users\Strutz\Documents\Daten\Science\hyades_mosaic.png 
REM  C:\Users\Strutz\Documents\Daten\science\stadtplan-museum-of-science-industry-1.ppm 
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\Cone-response.ppm 
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\dumper.ppm 
REM  C:\Users\Strutz\Documents\Daten\science\*.png                     
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\*.png                     
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\*.ppm                     
REM  C:\Users\Strutz\Documents\Daten\LCIC\*.ppm                     
REM  C:\Users\Strutz\Documents\Daten\IEETA.PT_ap\*.ppm                     
REM  C:\Users\Strutz\Documents\Daten\PWC-Corpus\*.ppm                     
REM  C:\Users\Strutz\Documents\Daten\TZ5_PNG\*.png                     
REM  C:\Users\Strutz\Documents\Daten\TZ5_PPM\*.ppm                     
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\Additional\*.png                     
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\Additional\*.pgm                     
REM  Re2Cl8-2-.ppm                                                                           
REM            7sPanda.ppm
REM ) do (


set res="res_test.txt"
set res_de="res_test_de.txt"
REM check all images from the beginning
set flag=1

REM 
echo #Results > %res%
echo #Results > %res_de%

REM ##################### Main Loop ##############################
for  %%I in (                               
	C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\*.ppm
)do (

 echo %%I
REM  echo .
REM  echo   %%I   >> %res%


REM pngtopnm %%I > orig.ppm

REM  %EXE% -i %%I -o tmp1.tsip -method CoBaLP2 -histMode comp -Idx 0  >> %res%

REM start with this images
 if "%%I" == "C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\Grey_720-360.ppm" (
 	set flag=1
  echo !flag!
 )

 if !flag! == 1 (

 echo %%I
 echo .
 echo   %%I   >> %res%
REM set name=%%I
REM echo %name%
 FOR /F "usebackq" %%A IN (%%I) DO set size=%%~zA
 echo size: %size%
REM call :processToken
 )
REM ##### end of main loop #########
)

pause
goto :ende

:processToken

REM FOR /F "tokens=4 delims= " %%S in ('dir /-C %temp% ^| find /i %%I') do (  

REM FOR /F "usebackq" %%A IN ('%name%') DO set size=%%~zA
set size='%%~zI'
REM 700x700*3 = 1500 000
 echo %name%
 echo size: %size%

pause
 if !size! LSS 1500000 (

  %EXE% -i %%I -o tmp1.tsip    >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> %res_de%
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

  %EXE% -i %%I -o tmp1.tsip  -method HXC   >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> %res_de%
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%


  %EXE% -i %%I -o tmp1.tsip  -method HXC  -histMode none -pred none >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> %res_de%
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

  %EXE% -i %%I -o tmp1.tsip  -method HXC  -histMode none -pred MEDPAETH >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

  %EXE% -i %%I -o tmp1.tsip  -method HXC -histMode comp -pred none >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

  %EXE% -i %%I -o tmp1.tsip  -method HXC  -histMode perm -pred none >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

  %EXE% -i %%I -o tmp1.tsip  -method HXC  -histMode comp -pred MEDPAETH >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

  %EXE% -i %%I -o tmp1.tsip  -method HXC  -histMode perm -pred MEDPAETH >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%


  %EXE% -i %%I -o tmp1.tsip  -method HXC -rct 1 -Idx 0 -histMode none -pred none >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

  %EXE% -i %%I -o tmp1.tsip  -method HXC -rct 1 -Idx 0 -histMode none -pred MEDPAETH >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

  %EXE% -i %%I -o tmp1.tsip  -method HXC -rct 1 -Idx 0 -histMode comp -pred none >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

  %EXE% -i %%I -o tmp1.tsip  -method HXC -rct 1 -Idx 0 -histMode perm -pred none >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%


  %EXE% -i %%I -o tmp1.tsip  -method BPC -rct 1 -Idx 0 -histMode comp -pred MEDPAETH >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

  %EXE% -i %%I -o tmp1.tsip  -method HXC -rct 1 -Idx 0 -histMode perm -pred MEDPAETH >> %res%
  %EXE% -i tmp1.tsip -o reco.ppm  >> res_de.txt
  %PSNR% -i1 %%I -i2 reco.ppm  -p  >> %res%

 )
REM)

goto :eof              
REM ":EOF"  führt die Batch-Datei nach dem Aufruf der Sprungmarke fort


:ende




pause

REM@echo on