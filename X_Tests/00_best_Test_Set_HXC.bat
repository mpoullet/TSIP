REM
 @echo off

setlocal enabledelayedexpansion


REM 
set EXE="C:\Users\Strutz\Documents\C\TSIPcoder_1_33\bin\TSIPcoder_1_33_.exe"
REM set EXE="..\bin_debug\TSIPcoder.exe"
set PSNR="C:\Users\Strutz\Documents\C\TSIPcoder_1_33\X_Tests\PSNR_C.exe"
set OutFile=tmpBTS.tsip
set LogFile=log_TSIP_Best_Set.txt


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




set res_en="res_test_Test_Set1.txt"
set flag=0

REM capture the current directory
set "currentDir=%cd%"

echo %currentDir%

REM change to directory with images
cd C:\Users\Strutz\Documents\Daten\CT2\Test_Set


%EXE% -i DarkWorld.ppm  -o %OutFile%  -log_name %LogFile% >> %res_en%
REM %EXE% -i DarkWorld.ppm  -o %OutFile% -tilesize 100000 -method tsip -idx 1 -idx_sorting 0 -skip_rlc1 1 -IFCreset 8 -IFCmax 3 -log_name %LogFile% >> %res_en%

%EXE% -i light_world-1_PNGOUT.ppm -o %OutFile%  -IFCmax 3  -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 24 -tolR3 2 -tolR4 36 -tolR5 35
set TOLG=-tolG0 0 -tolG1 0 -tolG2 24 -tolG3 2 -tolG4 36 -tolG5 35
set TOLB=-tolB0 0 -tolB1 0 -tolB2 24 -tolB3 2 -tolB4 36 -tolB5 35
set INCR=-HXC2increaseFac 1
%EXE% -i 01-03_rouders.ppm   -o %OutFile% -rct 1 -histMode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i langfristchart_historischer_chart_orangensaft_PNGOUT.ppm  -o %OutFile% -rct 1 -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i BumbleBee_HedKase.ppm  -o %OutFile% -tilesize 100000 -rct RGB -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1
set TOLG=-tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 1
set INCR=-HXC2increaseFac 2
%EXE% -i ladybug_landscape.png  -o tmpB1.tsip -rct 118 -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
%EXE% -i Wikispecies-logo.ppm  -o tmpB1.tsip -method HXC2  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 5 -tolR3 5 -tolR4 111 -tolR5 111
set TOLG=-tolG0 0 -tolG1 0 -tolG2 5 -tolG3 5 -tolG4 101 -tolG5 101
set TOLB=-tolB0 0 -tolB1 0 -tolB2 5 -tolB3 5 -tolB4 256 -tolB5 256
set INCR=-HXC2increaseFac 1
%EXE% -i tajmahal.ppm  -o tmpB1.tsip -rct 1 -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 5 -tolR3 5 -tolR4 220 -tolR5 220 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 5 -tolG3 5 -tolG4 220 -tolG5 220 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 5 -tolB3 5 -tolB4 220 -tolB5 220 
set INCR=-HXC2increaseFac 1
%EXE% -i map_vicinity.png  -o tmpB1.tsip  -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 4 -tolR1 4 -tolR2 4 -tolR3 4 -tolR4 4 -tolR5 4
set TOLG=-tolG0 4 -tolG1 4 -tolG2 4 -tolG3 4 -tolG4 4 -tolG5 4
set TOLB=-tolB0 4 -tolB1 4 -tolB2 4 -tolB3 4 -tolB4 4 -tolB5 4
set INCR=-HXC2increaseFac 1
%EXE% -i Landscape.png  -o tmpB1.tsip  -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 0 -tolG4 0 -tolG5 1
set TOLB=-tolB0 0 -tolB1 0 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1
set INCR=-HXC2increaseFac 2
%EXE% -i WEBDESIGNER_ISSUE_100.png  -o tmpB1.tsip -rct 1 -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en% 
set TOLR=-tolR0 0 -tolR1 0 -tolR2 48 -tolR3 48 -tolR4 55 -tolR5 55
set TOLG=-tolG0 0 -tolG1 0 -tolG2 48 -tolG3 48 -tolG4 56 -tolG5 56
set TOLB=-tolB0 0 -tolB1 0 -tolB2 52 -tolB3 52 -tolB4 58 -tolB5 58 
set INCR=-HXC2increaseFac 1
%EXE% -i Cone-response.ppm  -o tmpB1.tsip  -rct 1 -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 8 -tolR3 8 -tolR4 34 -tolR5 34
set TOLG=-tolG0 0 -tolG1 0 -tolG2 9 -tolG3 9 -tolG4 43 -tolG5 43
set TOLB=-tolB0 0 -tolB1 0 -tolB2 14 -tolB3 14 -tolB4 58 -tolB5 58 
set INCR=-HXC2increaseFac 1
%EXE% -i pixelart3jpg.png  -o tmpB1.tsip -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 41 -tolR3 41 -tolR4 88 -tolR5 88
set TOLG=-tolG0 0 -tolG1 0 -tolG2 64 -tolG3 64 -tolG4 180 -tolG5 180
set TOLB=-tolB0 0 -tolB1 0 -tolB2 52 -tolB3 52 -tolB4 52 -tolB5 52 
set INCR=-HXC2increaseFac 1
%EXE% -i SXI_20060706_151118162_AA_13_first.ppm  -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 3
%EXE% -i dumper.png  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 3
%EXE% -i PT_babeltower_01t1.png  -o %OutFile%  -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
%EXE% -i cubeiccp_PNGOUT.ppm  -o %OutFile% -rct 1 -method HXC2 -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 9 -tolR3 9 -tolR4 28 -tolR5 28 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 15 -tolG3 15 -tolG4 67 -tolG5 67 
set TOLB=-tolB0 1 -tolB1 1 -tolB2 46 -tolB3 46 -tolB4 254 -tolB5 254 
set INCR=-HXC2increaseFac 1
%EXE% -i P_thin_ext_cor_BlackBodyCT.ppm  -o %OutFile% -rct 1  -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 18 -tolR3 18 -tolR4 255 -tolR5 255
set TOLG=-tolG0 1 -tolG1 1 -tolG2 16 -tolG3 16 -tolG4 511 -tolG5 511
set TOLB=-tolB0 1 -tolB1 1 -tolB2 48 -tolB3 48 -tolB4 511 -tolB5 511
set INCR=-HXC2increaseFac 1
%EXE% -i Hangeul.ppm  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i chart2.ppm  -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 11 -tolR5 11 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 11 -tolG5 11 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 1 -tolB3 1 -tolB4 11 -tolB5 11 
set INCR=-HXC2increaseFac 1
%EXE% -i ecb-assembler-17t-1.png  -o %OutFile% -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 8 -tolR3 8 -tolR4 72 -tolR5 72 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 11 -tolG3 11 -tolG4 388 -tolG5 388 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 14 -tolB3 14 -tolB4 386 -tolB5 386 
set INCR=-HXC2increaseFac 1
%EXE% -i png.png  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 220 -tolR5 220 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 220 -tolG5 220 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 220 -tolB5 220 
set INCR=-HXC2increaseFac 1
%EXE% -i krb-insertpostersanta-07t.png  -o %OutFile% -rct 1 -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 1
%EXE% -i Xcode_Shortcuts_B_W.ppm  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i Xcode_Shortcuts_B_W.ppm  -o %OutFile% -HXC2auto 1 -method HXC2  -log_name %LogFile% >> %res_en%
REM %EXE% -i Xcode_Shortcuts_B_W.ppm  -o %OutFile% -tilesize 100000 -method tsip -histMode none -interleaving YY_uvuv -skip_rlc1 0 -postBWT mtf -MTF_val 0  -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 1
%EXE% -i orm-makeposter2008-14t.png  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i TF3D_Landscape.png  -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 1 -tolR1 1 -tolR2 4 -tolR3 4 -tolR4 4 -tolR5 4 
set TOLG=-tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1
set TOLB=-tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 
set INCR=-HXC2increaseFac 1
%EXE% -i ChartBuilder_PNGOUT.ppm  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 12 -tolR3 12 -tolR4 64 -tolR5 64 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 20 -tolG3 20 -tolG4 438 -tolG5 438
set TOLB=-tolB0 0 -tolB1 0 -tolB2 10 -tolB3 10 -tolB4 34 -tolB5 34 
set INCR=-HXC2increaseFac 1
%EXE% -i diabolik_001.png  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 3
%EXE% -i Farbkreis_Itten_1961.png  -o %OutFile% -rct 1 -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 1
%EXE% -i ciudad-2_0.png  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
%EXE% -i Tectonic_plates.png  -o %OutFile% -tilesize 100000 -method HXC2  -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 3
%EXE% -i mcn-cokedublin-43s.png  -o %OutFile% -rct A2_1 -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i png-IceAlpha-ie5mac.ppm  -o %OutFile% -method HXC2  -log_name %LogFile% >> %res_en%
%EXE% -i PTHNWIDE_WaveletsGamma_LVGamp16Contp8.ppm  -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%


set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 1
%EXE% -i police.png  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 221 -tolR3 221 -tolR4 256 -tolR5 256 
set TOLG=-tolG0 1 -tolG1 1 -tolG2 250 -tolG3 250 -tolG4 511 -tolG5 511 
set TOLB=-tolB0 5 -tolB1 5 -tolB2 92 -tolB3 92 -tolB4 511 -tolB5 511 
set INCR=-HXC2increaseFac 1
%EXE% -i Splenda-povray.ppm  -o %OutFile%  -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

REM %EXE% -i _vector-music-melody-preview_PNGOUT.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 2
%EXE% -i Xcode_Shortcuts.ppm  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%


set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 3
%EXE% -i text.ppm  -o %OutFile% -rct RGB -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 3
%EXE% -i save-png-dialog.png  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i 248080202_731e632c11_o.png  -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 24 -tolR3 24 -tolR4 41 -tolR5 41 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 166 -tolG3 166 -tolG4 470 -tolG5 470 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 9 -tolB3 9 -tolB4 259 -tolB5 259 
set INCR=-HXC2increaseFac 1
%EXE% -i noir-diabolik-guitar-hommess_design.png  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

REM %EXE% -i Nukleationskern_Fluessigkeit.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 3
%EXE% -i 500px-Roadmap_to_Unicode_BMP_de.png  -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i periodensystem-1280x887.png  -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%
%EXE% -i pie_charts_3d.ppm  -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%
%EXE% -i dev.ppm  -o %OutFile% -histmode comp -method HXC2 -log_name %LogFile% >> %res_en%
%EXE% -i Universe_expansion.png  -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 220 -tolR5 220 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 220 -tolG5 220 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 220 -tolB5 220 
set INCR=-HXC2increaseFac 1
%EXE% -i png-9passes.ppm  -o %OutFile% -histmode comp -method HXC2 -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 
set TOLG=-tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 
set TOLB=-tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 
set INCR=-HXC2increaseFac 1
%EXE% -i index_abc.png  -o %OutFile% -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

%EXE% -i Landscape_002.png  -o %OutFile% -method HXC2  -log_name %LogFile% >> %res_en%
%EXE% -i png-8passes_PNGOUT.ppm  -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 
set TOLG=-tolG0 2 -tolG1 2 -tolG2 2 -tolG3 2 -tolG4 1 -tolG5 1 
set TOLB=-tolB0 2 -tolB1 2 -tolB2 1 -tolB3 2 -tolB4 1 -tolB5 1 
set INCR=-HXC2increaseFac 1
%EXE% -i tangoIcons.png  -o %OutFile% -method HXC2 %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
%EXE% -i cover07_img10.png  -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%
%EXE% -i flaggen_580x2500_PNGOUT.ppm  -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%
%EXE% -i hp3.ppm  -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%

%EXE% -i ImageConverter.png  -o %OutFile% -rct A3_1 -method HXC2 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 255 -tolR3 255 -tolR4 255 -tolR5 255 
set TOLG=-tolG0 2 -tolG1 2 -tolG2 444 -tolG3 444 -tolG4 444 -tolG5 444 
set TOLB=-tolB0 17 -tolB1 14 -tolB2 228 -tolB3 228 -tolB4 228 -tolB5 228 
set INCR=-HXC2increaseFac 1
%EXE% -i BML_CAT_Chartres_03k_PNGOUT.ppm  -o %OutFile% -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 255 -tolR3 255 -tolR4 255 -tolR5 255 
set TOLG=-tolG0 2 -tolG1 2 -tolG2 444 -tolG3 444 -tolG4 444 -tolG5 444 
set TOLB=-tolB0 17 -tolB1 14 -tolB2 228 -tolB3 228 -tolB4 228 -tolB5 228 
set INCR=-HXC2increaseFac 1
%EXE% -i ecb_berlin_07t.png  -o %OutFile% -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 6 -tolR1 6 -tolR2 7 -tolR3 8 -tolR4 11 -tolR5 11
set TOLG=-tolG0 4 -tolG1 4 -tolG2 5 -tolG3 5 -tolG4 67 -tolG5 7
set TOLB=-tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1
set INCR=-HXC2increaseFac 1
%EXE% -i kbm-oslocentralxmas-34t.png  -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 11 -tolR1 11 -tolR2 16 -tolR3 16 -tolR4 22 -tolR5 22
set TOLG=-tolG0 9 -tolG1 9 -tolG2 11 -tolG3 11 -tolG4 15 -tolG5 15
set TOLB=-tolB0 3 -tolB1 3 -tolB2 5 -tolB3 5 -tolB4 6 -tolB5 6
set INCR=-HXC2increaseFac 1
%EXE% -i FRD-Bonn-20s-882x623.ppm  -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 2 -tolR1 2 -tolR2 2 -tolR3 2 -tolR4 44 -tolR5 44 
set TOLG=-tolG0 2 -tolG1 2 -tolG2 2 -tolG3 2 -tolG4 44 -tolG5 44 
set TOLB=-tolB0 2 -tolB1 2 -tolB2 2 -tolB3 2 -tolB4 44 -tolB5 44 
set INCR=-HXC2increaseFac 1
%EXE% -i Map_118230550922074.ppm -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 
set TOLG=-tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 
set TOLB=-tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 
set INCR=-HXC2increaseFac 1
%EXE% -i ecb_ny_89t.png  -o %OutFile% -histmode none -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i reactos_0.3.1_booklet_cover.png -o %OutFile% -method HXC2 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 110 -tolR5 110
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 110 -tolG5 110
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 110 -tolB5 110
set INCR=-HXC2increaseFac 1
%EXE% -i europa_karte_de_PNGOUT.ppm -o %OutFile% -rct 1 -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
%EXE% -i Tectonic_plates.png  -o %OutFile% -method HXC2  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 44 -tolR3 44 -tolR4 214 -tolR5 214
set TOLG=-tolG0 12 -tolG1 12 -tolG2 189 -tolG3 189 -tolG4 318 -tolG5 318
set TOLB=-tolB0 0 -tolB1 0 -tolB2 17 -tolB3 17 -tolB4 72 -tolB5 72
set INCR=-HXC2increaseFac 1
%EXE% -i tux-agafix.png  -o %OutFile%  -method HXC2 %TOLR% %TOLG% %TOLB% %INCR%   -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 
set INCR=-HXC2increaseFac 1
%EXE% -i Xcode_Shortcuts_B_W.ppm  -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

if !flag! == 1 (
%EXE% -i _vector-glassy-button-cs-by-dragonart.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i 03PNG-PNGOUTWin.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 20080227053020!Myoglobin_PNGOUT.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i 02_Schluesselfelder_Schiff-PNG.ppm -histMode comp -method Cobalp2 -sT 0 -o %OutFile%  -log_name %LogFile% >> %res_en%
%EXE% -i 2mpampjjpg.png  -o %OutFile% -method Cobalp2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i lunar-lander.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i cwheel.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i 618px-Speed1c.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i 20_petra_cold_landscape.png  -o %OutFile% -histmode none -method CoBaLP2 -sT 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i eagle.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i captured_Image-png_4d60c4b0-7133-4ef8-bbe0-be0e27b0bf29_PNGOUT.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i grey-leaves-1g7u_small.ppm  -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i Ashton_landscape.png -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i BigBuckBunny-Landscape.png -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i coffee2-216x300.png -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i Desert_landscape.png -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i diabolik_002.png -o %OutFile% -method CoBaLP2 -sT 1 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i disneyBMP.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i fragerance1.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i Gem-BMP_1000.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i Landscape_Full_HD_by_depARTed89.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i Merian-PNG.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i ImageConverter_1.png  -o tmpB2.tsip -method tsip -histMode none -pred ADAPT -interleaving YY_uvuv -skip_rlc1 0 -RLC2_zeros 0 -IFCreset 5000 -IFCmax 300 -log_name %LogFile% >> %res_en%
%EXE% -i mediothek_landscape.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i Merian-PNG.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i Oblivion2007101513345379.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i Oblivion2007101714023034.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i Oblivion2007101714024170.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i Oblivion2007101714040018.png -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i pasodoble1.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i pasodoble3.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i prismIcon.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i ScreenShot37.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i ScreenShot39.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i soldier_x.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i stockschtze.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i supersexygirls-pack-2.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i TIC_Wallpaper001.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 02PNG-PNGOUTWin.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 02_Schluesselfelder_Schiff-PNG.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i reactos_0.3.1_booklet_cover.png -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 275003.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 275004.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en% 
%EXE% -i 275005.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 275006.ppm -o %OutFile% -histmode comp -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 275007.ppm -o %OutFile% -histmode none -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 275009.ppm -o %OutFile% -histmode comp -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 275008.ppm -o %OutFile% -histmode none -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 275002.ppm -o %OutFile% -histmode none -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en% 
%EXE% -i 275000.ppm -o %OutFile% -histmode comp -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 275001.ppm -o %OutFile% -histmode comp -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 354000.ppm -o %OutFile% -rct 84 -histmode none -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 354001.ppm -o %OutFile% -histmode none -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 354002.ppm -o %OutFile% -histmode none -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 354003.ppm -o %OutFile% -histmode none -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 354004.ppm -o %OutFile% -histmode none -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 354005.ppm -o %OutFile% -histmode none -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i MMSposter-large.png -o %OutFile% -histmode none -method CoBaLP2 -sT 1 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i MMSposter-large.png -o %OutFile% -histmode comp -method cobalp2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i 354006.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 354007.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 354008.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 354009.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 384001.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 384002.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 384003.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 384004.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 384006.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 384007.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 384009.ppm -o %OutFile% -histmode none -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 384010-2.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 384010.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 384011.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i 800px-0849_pilar_ebro_2004.ppm -o %OutFile% -rct 98 -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en% 
%EXE% -i Alexandra+Burke+SUPER+HQ++PNG.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i architecture-5ao.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i architecture-7q7.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i artificial8_full_PNGOUT.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i kodim01.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim02.ppm -o %OutFile% -rct 13 -log_name %LogFile% >> %res_en%
%EXE% -i kodim03.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim04.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim05.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim06.ppm -o %OutFile% -rct 49 -log_name %LogFile% >> %res_en%
%EXE% -i kodim07.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim08.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim09.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim10.ppm -o %OutFile% -rct 83 -log_name %LogFile% >> %res_en%
%EXE% -i kodim11.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim12.ppm -o %OutFile% -rct 11 -log_name %LogFile% >> %res_en%
%EXE% -i kodim13.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim14.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim15.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim16.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim17.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim18.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim19.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim20.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim21.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim22.ppm -o %OutFile% -rct 17 -log_name %LogFile% >> %res_en%
%EXE% -i kodim23.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i kodim24.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i barbara_PNGOUT.ppm -o %OutFile% -rct 74 -interleaving Y_U_V -method TSIP -skip_rlc 1  -log_name %LogFile% >> %res_en%
%EXE% -i barbara2_PNGOUT.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i auto1.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i balloon.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i beach1.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i beach2.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i bear.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i bike.rgb_PNGOUT.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i bike3.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i BlockMap_Photoshop_PNGOUT.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i aspens.ppm -o %OutFile% -method TSIP -idx 1 -idx_sorting 1 -skip_rlc1 1  -IFCreset 128 -IFCmax 32 -log_name %LogFile% >> %res_en%
%EXE% -i battery-6rc0.ppm -o %OutFile% -histmode comp -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i boots_PNGOUT.ppm -o %OutFile% -pred Medpaeth -interleaving YY_uvuv -method TSIP -skip_rlc1 0 -log_name %LogFile% >> %res_en%
%EXE% -i reba.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i redhead.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i roof-uh8k.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i rope-239.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i rubbish-water-2ju8.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i satellite.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i saturn.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i seagull.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i ship.rgb.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i soft-drinks-7z2.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i Spring1887.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i steamtrain.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i stone-i9sv.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i sunflower.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i texture-009.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i The+Saturdays+Better+Quality+in+PNG.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i The+Veronicas+PNG.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i tiger.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i tree-32.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i tree-794.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i wallpaper_oceanfreight_1280x960.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i wallpaper_railfreight_1280x960.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i water-08.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i water-dx3d.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i wolf.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i woman_rgb.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i _vector-glassy-button-cs-by-dragonart.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i _vector-music-melody-preview_PNGOUT.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i black.ppm -o %OutFile% -rct 77 -interleaving YY_uuvv -pred ADAPT -method TSIP -skip_rlc1 0 -log_name %LogFile% >> %res_en%
%EXE% -i bullfight.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i cafe.rgb.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i carpets-j4h.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i Cevennes2.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i chinook.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i colours-10.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i colours-an5.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i computer-301.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i computer-309.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i construction-06.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i construction-g6vr.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i construction-w7xq.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i corn-j7z2.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i couple.ppm -o %OutFile% -rct 110 -method CoBaLP2 -sA 1 -log_name %LogFile% >> %res_en%
%EXE% -i crowd-people-g3ky.ppm -log_name %LogFile% >> %res_en%
%EXE% -i demos_gift-demo_PNGOUT.ppm -log_name %LogFile% >> %res_en%
%EXE% -i didle_PNGOUT.ppm -log_name %LogFile% >> %res_en%
%EXE% -i door-e5f8.ppm -log_name %LogFile% >> %res_en%
%EXE% -i demos_explorer_PNGOUT.ppm -log_name %LogFile% >> %res_en%
%EXE% -i Facade.ppm -log_name %LogFile% >> %res_en%
%EXE% -i face1.ppm -log_name %LogFile% >> %res_en%
%EXE% -i favicons_large_PNGOUT.ppm -log_name %LogFile% >> %res_en%
%EXE% -i flowers-y02o.ppm -log_name %LogFile% >> %res_en%
%EXE% -i gg.ppm -log_name %LogFile% >> %res_en%
%EXE% -i happybirds.ppm -log_name %LogFile% >> %res_en%
%EXE% -i home3.ppm -log_name %LogFile% >> %res_en%
%EXE% -i home4.ppm -log_name %LogFile% >> %res_en%
%EXE% -i im1.ppm -log_name %LogFile% >> %res_en%
%EXE% -i im10.ppm -log_name %LogFile% >> %res_en%
%EXE% -i im12.ppm -log_name %LogFile% >> %res_en%
%EXE% -i im14.ppm -log_name %LogFile% >> %res_en%
%EXE% -i im15.ppm -log_name %LogFile% >> %res_en%
%EXE% -i im19.ppm -log_name %LogFile% >> %res_en%
%EXE% -i im2.ppm -log_name %LogFile% >> %res_en%
%EXE% -i im20.ppm -log_name %LogFile% >> %res_en%
%EXE% -i im4.ppm -log_name %LogFile% >> %res_en%
%EXE% -i im7.ppm -log_name %LogFile% >> %res_en%
%EXE% -i img-icons-a-png-official-macosx-leopard-icon-pack-daddy77-17777.ppm -log_name %LogFile% >> %res_en%
%EXE% -i Jupiter_Earth_Comparison.ppm -log_name %LogFile% >> %res_en%
%EXE% -i window1.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i zoo-016.ppm -o %OutFile% -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i flowers-k5cq.ppm -o %OutFile% -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i girl.ppm -o %OutFile% -rct 1 -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i girl1.ppm -o %OutFile% -rct 116 -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i im3.ppm -o %OutFile% -method LOCO -log_name %LogFile% >> %res_en%
%EXE% -i lake1.ppm -o %OutFile% -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i Bretagne2.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 1 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i brick.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 1 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i pc-z5e2.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i cathedral.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i people-126.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i plant-fol2.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i puzzle-p7z.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i pzbr11_0.16.10_PNGOUT.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i cement.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 1 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i Cevennes1.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i server.ppm -o %OutFile% -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i shadow.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i stones-cairns-76.ppm -o %OutFile% -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i The_Earth_seen_from_Apollo_17_PNGOUT.ppm -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i tree-flowers-u6v.ppm -o %OutFile% -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i misc-09.ppm -o %OutFile% -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i o-png24_PNGOUT.ppm -o %OutFile% -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i p08_PNGOUT.ppm -o %OutFile% -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i Lady+Gaga+PNG.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i lawn-v6h3.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i leaves-12.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i leaves-6n3y.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i leaves-kijj7.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i leaves-mval_small.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i macro-765.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i Michelangelo_David.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i night-379.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i party_4s.ppm -o %OutFile% -log_name %LogFile% >> %res_en%
%EXE% -i p08_PNGOUT.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i im5.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i im6.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i im8.ppm -o %OutFile% -method CoBaLP2 -sT 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i pebbles.ppm -o %OutFile% -method CoBaLP2 -sT 1 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i pebbles.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 1 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i lake1.ppm -o %OutFile% -tilesize 1000000 -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i rapeseed-z83.ppm -o %OutFile% -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -i The_Earth_seen_from_Apollo_17_PNGOUT.ppm -o %OutFile% -tilesize 1000000 -histmode comp -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%

)



cd C:\Users\Strutz\Documents\Daten\CT2\Test_Set\Not-HXC


if !flag! == 1 (

echo end of file list

)
 


REM go back to initial directory
cd %currentDir%


pause

REM@echo on