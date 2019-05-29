REM 
 @echo off

setlocal enabledelayedexpansion


REM 
set EXE="C:\Users\Strutz\Documents\C\TSIPcoder_1_34\bin\TSIPcoder_1_34.exe"
REM set EXE="..\bin_debug\TSIPcoder.exe"
set PSNR="PSNR_C.exe"

set OutFile=tmpbTS3.tsip
set LogFile=log_TSIP_Best_Wiki.txt

REM dir   \B    C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\*.ppm     > filenames.txt 


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




set res_en="res_test_Wiki.txt"
set flag=0

REM capture the current directory
set "currentDir=%cd%"

echo %currentDir%

REM change to directory with images
cd C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM
cd C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM


if !flag! == 1 (
echo
set TOLR=-tolR0 0 -tolR1 0 -tolR2 38 -tolR3 38 -tolR4 59 -tolR5 59
set TOLG=-tolG0 1 -tolG1 1 -tolG2  5 -tolG3  5 -tolG4 5 -tolG5 5
set TOLB=-tolB0 1 -tolB1 1 -tolB2  6 -tolB3  6 -tolB4 7 -tolB5 7
set INCR=-HXC2increaseFac 1
%EXE% -i -Re2Cl8-2-.ppm -o %OutFile% -rct 2 -histmode none	-method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1
set TOLG=-tolG0 1 -tolG1 1 -tolG2 5 -tolG3 5 -tolG4 5 -tolG5 5
set TOLB=-tolB0 1 -tolB1 1 -tolB2 6 -tolB3 6 -tolB4 7 -tolB5 7
set INCR=-HXC2increaseFac 1
%EXE% -i 1024px-ComputerMemoryHierarchy.ppm -o %OutFile%	-method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i 1024px-Iceweasel9.0.1.ppm	-o %OutFile% -rct A3_1 -histmode none	-method BPC -pred PAETH	-log_name %LogFile% >> %res_en%

set TOLR=-tolR0 1 -tolR1 1 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set INCR=-HXC2increaseFac 1
%EXE% -i 1024px-Table-3.ppm -o %OutFile%	-method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name  >> %res_en%
set TOLR=-tolR0 4 -tolR1 4 -tolR2 17 -tolR3 17 -tolR4 26 -tolR5 26
set TOLG=-tolG0 12 -tolG1 12 -tolG2 36 -tolG3 26 -tolG4 88 -tolG5 88
set TOLB=-tolB0 4 -tolB1 4 -tolB2 9 -tolB3 9 -tolB4 19 -tolB5 19
set INCR=-HXC2increaseFac 1
%EXE% -i 160SignalBdeDUI.ppm	-o %OutFile% -rct A3_9 -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i 1821_Bandera_Trigarante_AGN.ppm	-o %OutFile% -idx 1 -idx_sorting 0 -histmode perm	-method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 20 -tolR1 20 -tolR2 66 -tolR3 66 -tolR4 111 -tolR5 111
set INCR=-HXC2increaseFac 1
%EXE% -i "267px-Surcoat_(PSF).ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i 2-Methylpentane_1.ppm	-o %OutFile%	-histmode comp -method cobalp2  -sT 0  -log_name %LogFile% >> %res_en%
%EXE% -i 387px-Microburil_2.ppm -o %OutFile% -histmode comp -method cobalp2  -sT 0 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i 400px-trigramme.ppm	-o %OutFile% -rct hp1 -histmode none	-method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
%EXE% -i 454px-Phillips_screwdriver_1.ppm -o %OutFile%	-histmode comp -method cobalp2  -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i "492px-Wappen_Heessel_(Burgdorf).ppm"	-o %OutFile% -histmode comp -method cobalp2  -sT 0 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1
set TOLB=-tolB0 0 -tolB1 0 -tolB2 9 -tolB3 9 -tolB4 9 -tolB5 9
set INCR=-HXC2increaseFac 1
%EXE% -i "500px-Flag_of_Ukrainian_SSR_(1937-1949).svg.ppm"	-o %OutFile% -rct A3_6 -histmode comp	-method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i 582px-Pfeffer_Osmotische_Untersuchungen-5-2.ppm -o %OutFile% -histmode comp -method cobalp2 -sT 0	-log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1
set TOLB=-tolB0 0 -tolB1 0 -tolB2 11 -tolB3 11 -tolB4 90 -tolB5 90
set INCR=-HXC2increaseFac 1
%EXE% -i 600px-Flag_of_Ukrainian_SSR.svg.ppm -o %OutFile%	-rct A3_8 -histmode comp	-method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i "615px-Pannier_(PSF).ppm" -o %OutFile% -histmode none -method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 8 -tolR1 10 -tolR2 7 -tolR3 10 -tolR4 9 -tolR5 10
set TOLG=-tolG0 6 -tolG1 6 -tolG2 7 -tolG3 7 -tolG4 7 -tolG5 7
set TOLB=-tolB0 7 -tolB1 7 -tolB2 8 -tolB3 8 -tolB4 8 -tolB5 8
set INCR=-HXC2increaseFac 1
%EXE% -i "601px-Frangn(2).ppm" -o %OutFile%	-rct A2_1 -histmode none	-method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 13 -tolR1 13 -tolR2 33 -tolR3 33 -tolR4 33 -tolR5 33
set INCR=-HXC2increaseFac 1
%EXE% -i "613px-Scissors2_(PSF).ppm"	-o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% 	-log_name %LogFile% >> %res_en%
set TOLR=-tolR0 4 -tolR1 5 -tolR2 7 -tolR3 5 -tolR4 4 -tolR5 5
set INCR=-HXC2increaseFac 1
%EXE% -i "620px-Dynamo_(PSF).ppm" -o %OutFile% -histmode none	-method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 2 -tolR1 2 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set INCR=-HXC2increaseFac 3
%EXE% -i 640px-ETO..ppm -o %OutFile% -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i "640px-IJ_(letter).ppm" -o %OutFile% -histmode none -method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 1 -tolG5 1
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 1 -tolB5 1
set INCR=-HXC2increaseFac 1
%EXE% -i 640px-SSadditiveblock.ppm -o %OutFile% -rct A2_3 -histmode comp -pred MEDPAETH -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 20 -tolR1 20 -tolR2 95 -tolR3 95 -tolR4 95 -tolR5 95
set INCR=-HXC2increaseFac 1
%EXE% -i "649px-Sponge_(PSF).ppm" -o %OutFile% -histmode none	-method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 2 -tolR2 1 -tolR3 1 -tolR4 2 -tolR5 2
set TOLG=-tolG0 2 -tolG1 2 -tolG2 2 -tolG3 3 -tolG4 3 -tolG5 3
set TOLB=-tolB0 3 -tolB1 3 -tolB2 3 -tolB3 3 -tolB4 3 -tolB5 3
set INCR=-HXC2increaseFac 1
%EXE% -i 653px-MieMapCurrent.ppm -o %OutFile% -rct A3_12 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 16 -tolR1 16 -tolR2 55 -tolR3 55 -tolR4 66 -tolR5 66
set INCR=-HXC2increaseFac 1
%EXE% -i "666px-Tandem_(PSF).ppm"	-o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% 	-log_name %LogFile% >> %res_en%
set TOLR=-tolR0 2 -tolR1 2 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set TOLG=-tolG0 5 -tolG1 3 -tolG2 5 -tolG3 7 -tolG4 6 -tolG5 5
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i "674px-Map_of_south-west_England_shown_within_England_(Met_Office_region).ppm" -o %OutFile% -rct A2_1 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 2 -tolR1 2 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set TOLG=-tolG0 2 -tolG1 3 -tolG2 2 -tolG3 3 -tolG4 2 -tolG5 2
set TOLB=-tolB0 2 -tolB1 2 -tolB2 2 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i 682px-Baden.ppm -o %OutFile% -rct A6_11 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 2 -tolR3 2 -tolR4 3 -tolR5 3
set TOLG=-tolG0 0 -tolG1 0 -tolG2 2 -tolG3 2 -tolG4 3 -tolG5 3
set TOLB=-tolB0 0 -tolB1 0 -tolB2 2 -tolB3 2 -tolB4 3 -tolB5 3
set INCR=-HXC2increaseFac 3
%EXE% -i "686px-TSS_2.ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 2 -tolR5 2
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 2 -tolG5 2
set TOLB=-tolB0 0 -tolB1 0 -tolB2 1 -tolB3 1 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i 689px-Screens_png.ppm -o %OutFile% -rct A3_11 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set TOLG=-tolG0 3 -tolG1 3 -tolG2 3 -tolG3 3 -tolG4 3 -tolG5 3
set TOLB=-tolB0 3 -tolB1 3 -tolB2 3 -tolB3 3 -tolB4 3 -tolB5 3
set INCR=-HXC2increaseFac 1
%EXE% -i 719px-FukuokaMapCurrent.ppm -o %OutFile% -rct A3_12 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 7 -tolR3 7 -tolR4 7 -tolR5 7
set TOLG=-tolG0 1 -tolG1 1 -tolG2 19 -tolG3 19 -tolG4 49 -tolG5 49
set TOLB=-tolB0 1 -tolB1 1 -tolB2 21 -tolB3 21 -tolB4 21 -tolB5 21
set INCR=-HXC2increaseFac 1
%EXE% -i 726px-Map_of_Old_Great_Bulgaria_version_german_text.svg.ppm -o %OutFile% -rct A5_11 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i 733px-GifuMapCurrent.ppm -o %OutFile% -rct A6_12 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 2 -tolR1 2 -tolR2 3 -tolR3 3 -tolR4 2 -tolR5 2
set TOLG=-tolG0 3 -tolG1 3 -tolG2 3 -tolG3 3 -tolG4 3 -tolG5 3
set TOLB=-tolB0 2 -tolB1 2 -tolB2 2 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i 772px-2wdpanc_1.ppm -o %OutFile% -rct A2_11 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i 793px-Bodhidharma_..ppm -o %OutFile% -rct A3_1  -histmode comp -method cobalp2 -sT 0	-log_name %LogFile% >> %res_en%
%EXE% -i 793px-Fig.2.3.ppm -o %OutFile% -rct Pei09 -method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i 795px-Phillips_screw_head_2.ppm -o %OutFile% -rct A3_1 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Colgate_(logo).ppm" -o %OutFile% -rct B1_2 -histmode comp  -method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 3 -tolR3 3 -tolR4 6 -tolR5 6
set INCR=-HXC2increaseFac 1
%EXE% -i 800px-C.ppm -o %OutFile% -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i "1024px-Hemispheres_(PSF).ppm" -o %OutFile% -histmode none -method CoBalp2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i 634px-Wiesbaden_districts.ppm -o %OutFile% -rct A3_9 -histmode comp -method cobalp2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i 640px-Bicycle_rim_diagrams_02_.ppm -o %OutFile% -rct A2_1 -histmode comp -method cobalp2 -sT 0 -log_name %LogFile% >> %res_en%
%EXE% -i 800px-CompizNova2.1.ppm -o %OutFile% -histmode comp -method cobalp2 -st 0	-log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 2 -tolR4 2 -tolR5 2
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 2 -tolG4 2 -tolG5 2
set TOLB=-tolB0 0 -tolB1 0 -tolB2 1 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 3
%EXE% -i 800px-Cow-2.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i 800px-Diap3.1.ppm -o %OutFile% -histmode comp -method cobalp2 -st 0 -log_name %LogFile% >> %res_en%
REM %EXE% -i 800px-Hunnenwanderung.ppm -o %OutFile%	-histmode comp -method cobalp2 -st 0  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 110 -tolR3 110 -tolR4 110 -tolR5 110
set TOLG=-tolG0 0 -tolG1 0 -tolG2 11 -tolG3 11 -tolG4 44 -tolG5 44
set TOLB=-tolB0 0 -tolB1 0 -tolB2 2 -tolB3 2 -tolB4 44 -tolB5 44
set INCR=-HXC2increaseFac 1
%EXE% -i 800px-LOM500_1.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 3 -tolR1 3 -tolR2 5 -tolR3 5 -tolR4 5 -tolR5 5
set TOLG=-tolG0 3 -tolG1 3 -tolG2 5 -tolG3 5 -tolG4 5 -tolG5 5
set TOLB=-tolB0 5 -tolB1 5 -tolB2 7 -tolB3 7 -tolB4 8 -tolB5 8
set INCR=-HXC2increaseFac 1
%EXE% -i 800px-MediaWikiExternalEditorImage.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 11 -tolR3 11 -tolR4 17 -tolR5 17
set TOLG=-tolG0 1 -tolG1 1 -tolG2 10 -tolG3 10 -tolG4 15 -tolG5 15
set TOLB=-tolB0 1 -tolB1 1 -tolB2 11 -tolB3 11 -tolB4 44 -tolB5 44
set INCR=-HXC2increaseFac 1
%EXE% -i "800px-Nickerie_(2).ppm" -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
REM %EXE% -i 800px-Spur3_(PSF).ppm -o %OutFile% -histmode comp -method cobalp2 -st 0 -log_name %LogFile% >> %res_en%
REM %EXE% -i 800px-PFCS_.ppm -o %OutFile% -histmode comp -method cobalp2 -st 0	-log_name %LogFile% >> %res_en%
%EXE% -i "800px-Piazza_(PSF).ppm" -o %OutFile% -histmode none -method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 1 -tolR2 0 -tolR3 1 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 1
%EXE% -i 800px-SSadditiveblock.ppm -o %OutFile%  -histmode comp -pred MEDPAETH -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
REM %EXE% -i 800px-SvobodaOver5%.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1
set TOLG=-tolG0 2 -tolG1 2 -tolG2 2 -tolG3 2 -tolG4 2 -tolG5 2
set TOLB=-tolB0 2 -tolB1 2 -tolB2 2 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i 800px-TEDxPune_Logo_.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 11 -tolR3 11 -tolR4 80 -tolR5 80
set TOLG=-tolG0 8 -tolG1 8 -tolG2 8 -tolG3 8 -tolG4 8 -tolG5 8
set TOLB=-tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1
set INCR=-HXC2increaseFac 1
%EXE% -i 800px-Sporskifte_2.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i 800px-Crankshaft_2.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 30 -tolR3 30 -tolR4 30 -tolR5 30
set TOLG=-tolG0 0 -tolG1 0 -tolG2 6 -tolG3 6 -tolG4 10 -tolG5 10
set TOLB=-tolB0 0 -tolB1 0 -tolB2 6 -tolB3 6 -tolB4 8 -tolB5 8
set INCR=-HXC2increaseFac 1
%EXE% -i 800px-FBReader.0.10.7.ppm -o %OutFile%	-histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i 800px-Fig_4.4.ppm -o %OutFile%	-histmode none -Idx 0 -method BPC -pred PAETH -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 4 -tolR1 4 -tolR2 10 -tolR3 10 -tolR4 10 -tolR5 10
set INCR=-HXC2increaseFac 1
%EXE% -i "800px-Thorax_(PSF).ppm" -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 2 -tolR5 2
set TOLG=-tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 2 -tolG5 2
set TOLB=-tolB0 1 -tolB1 1 -tolB2 2 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i "800px-Wagon_(Combi).ppm" -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 20 -tolR5 20
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 20 -tolG5 20
set TOLB=-tolB0 0 -tolB1 0 -tolB2 2 -tolB3 2 -tolB4 20 -tolB5 20
set INCR=-HXC2increaseFac 1
%EXE% -i 819px-Montenegroetno03.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

REM %EXE% -i "857px-Frame_(PSF).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i 889px-Pozi_screw_head_2.ppm -o %OutFile% -histmode comp -method cobalp2 -st 0 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 1 -tolR5 1
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 6 -tolG5 6
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 1 -tolB5 1
set INCR=-HXC2increaseFac 1
%EXE% -i A-4.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i A-4.ppm -o %OutFile% -histmode comp -method HXC2  -HXC2auto 1 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i A42.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i Abadie.jo-Forme-1.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i 960px-Mynd_1.ppm -o %OutFile% -histmode none -Idx 0 -method BPC -pred PAETH -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 1
%EXE% -i AbortionLawsMap.ppm -o %OutFile% -rct A6_1 -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i Achsensymmetrie.ppm -o %OutFile% -idx 1 -idx_sorting 0 -method HXC -tol 0 -tolOff1 0 -tolOff2 0 -HXCincreasefac 5	-log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 1 -tolR5 1
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 1 -tolG5 1
set TOLB=-tolB0 0 -tolB1 0 -tolB2 2 -tolB3 2 -tolB4 4 -tolB5 4
set INCR=-HXC2increaseFac 1
%EXE% -i Acid2Firefox3.0.1.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 3 -tolR5 3
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 6 -tolG5 6
set TOLB=-tolB0 0 -tolB1 0 -tolB2 1 -tolB3 1 -tolB4 4 -tolB5 4
set INCR=-HXC2increaseFac 1
%EXE% -i Achsensymmetrie.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%	-log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 1 -tolR5 1
set TOLG=-tolG0 1 -tolG1 1 -tolG2 2 -tolG3 2 -tolG4 3 -tolG5 3
set TOLB=-tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1
set INCR=-HXC2increaseFac 1
%EXE% -i Acid3detailFirefox3_6.ppm -o %OutFile% -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i Acid3detailFirefox3_6.ppm -o %OutFile% -histmode comp -method HXC2  -hxc2auto 1 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i Acid3Fx3_5.ppm -o %OutFile% -rct RGB -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 7 -tolR3 7 -tolR4 7 -tolR5 7
set TOLG=-tolG0 1 -tolG1 1 -tolG2 4 -tolG3 4 -tolG4 4 -tolG5 4
set TOLB=-tolB0 0 -tolB1 0 -tolB2 3 -tolB3 3 -tolB4 5 -tolB5 5
set INCR=-HXC2increaseFac 1
%EXE% -i Acid3iPod1.1.4.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 4 -tolR1 4 -tolR2 24 -tolR3 24 -tolR4 24 -tolR5 24
set INCR=-HXC2increaseFac 1
%EXE% -i Acting...ppm -o %OutFile% -histmode comp -methhod HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 20 -tolR3 20 -tolR4 41 -tolR5 41
set INCR=-HXC2increaseFac 1
%EXE% -i Adminpedia.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 10 -tolR3 10 -tolR4 10 -tolR5 10
set TOLG=-tolG0 0 -tolG1 0 -tolG2 9 -tolG3 9 -tolG4 10 -tolG5 10
set TOLB=-tolB0 0 -tolB1 0 -tolB2 20 -tolB3 20 -tolB4 10 -tolB5 10
set INCR=-HXC2increaseFac 3
%EXE% -i "Aerocapture_(de).ppm" -o %OutFile% -histmode comp -method HXC2 -hxc2auto 1 %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 1 -tolR5 1
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 1 -tolG5 1
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 2 -tolB5 1
set INCR=-HXC2increaseFac 1
%EXE% -i Adminer03.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i "AgeStructureUa1897+.ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i AHPHierarchy02_1.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i AHPHierarchy4_0.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set TOLG=-tolG0 0 -tolG1 0 -tolG2 2 -tolG3 2 -tolG4 2 -tolG5 2
set TOLB=-tolB0 0 -tolB1 0 -tolB2 2 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i "Alosizes(v2).ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 11 -tolR3 1 -tolR4 111 -tolR5 111
set TOLG=-tolG0 1 -tolG1 1 -tolG2 44 -tolG3 44 -tolG4 111 -tolG5 111
set TOLB=-tolB0 1 -tolB1 1 -tolB2 22 -tolB3 22 -tolB4 111 -tolB5 111
set INCR=-HXC2increaseFac 1
%EXE% -i Aimags.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 25 -tolR3 25 -tolR4 56 -tolR5 56
set TOLG=-tolG0 8 -tolG1 8 -tolG2 17 -tolG3 17 -tolG4 17 -tolG5 17
set TOLB=-tolB0 1 -tolB1 1 -tolB2 2 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i "ARA_.ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
REM %EXE% -i Aragon.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 12 -tolR3 12 -tolR4 33 -tolR5 33
set TOLG=-tolG0 1 -tolG1 1 -tolG2 11 -tolG3 11 -tolG4 41 -tolG5 41
set TOLB=-tolB0 0 -tolB1 0 -tolB2 20 -tolB3 20 -tolB4 32 -tolB5 32
set INCR=-HXC2increaseFac 1
%EXE% -i Arsbreytinglandsfr2000-8.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
REM %EXE% -i Ambientocclusion-.ppm -o %OutFile% -histmode comp -method cobalp2 -st 0 -log_name %LogFile% >> %res_en%
%EXE% -i Android-2.3.ppm -o %OutFile% -Idx 1 -idx_sorting 0 -histmode none -method TSIP  -skip_rlc1 1 -IFCreset 10 -IFCmax 3 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i BALLZ.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i Baltic_languages_PL.ppm -o %OutFile% -histmode comp -method cobalp2 -st 0 -log_name %LogFile% >> %res_en%
%EXE% -i Barnstar_of_High_Culture.ppm -o %OutFile% -histmode none -method LOCO  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i Banner_2.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 7 -tolR3 7 -tolR4 7 -tolR5 7
set TOLG=-tolG0 1 -tolG1 1 -tolG2 7 -tolG3 7 -tolG4 7 -tolG5 7
set TOLB=-tolB0 1 -tolB1 1 -tolB2 8 -tolB3 8 -tolB4 8 -tolB5 8
set INCR=-HXC2increaseFac 1
%EXE% -i Be-map.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 4 -tolR1 4 -tolR2 23 -tolR3 23 -tolR4 64 -tolR5 64
set TOLG=-tolG0 1 -tolG1 1 -tolG2 17 -tolG3 17 -tolG4 44 -tolG5 44
set TOLB=-tolB0 5 -tolB1 5 -tolB2 13 -tolB3 13 -tolB4 84 -tolB5 84
set INCR=-HXC2increaseFac 1
%EXE% -i BeirutMap_.ppm -o %OutFile% -histmode none -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 3 -tolR3 3 -tolR4 5 -tolR5 5
set TOLG=-tolG0 0 -tolG1 0 -tolG2 12 -tolG3 12 -tolG4 23  -tolG5 23
set TOLB=-tolB0 0 -tolB1 0 -tolB2 6 -tolB3 6 -tolB4 11 -tolB5 11
set INCR=-HXC2increaseFac 1
%EXE% -i Bellamy_2.ppm -o %OutFile% -histmode none -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 2 -tolR3 1 -tolR4 1 -tolR5 1
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 0 -tolG4 1 -tolG5 1
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 1
set INCR=-HXC2increaseFac 1
%EXE% -i Bellamy_3.ppm -o %OutFile% -histmode none -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i BeharQamar.ppm -o %OutFile%  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i Benzaldehyde.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 53 -tolR1 53 -tolR2 53 -tolR3 53 -tolR4 53 -tolR5 53
set TOLG=-tolG0 5 -tolG1 5 -tolG2 47 -tolG3 47 -tolG4 47 -tolG5 47
set TOLB=-tolB0 1 -tolB1 1 -tolB2 61 -tolB3 61 -tolB4 61 -tolB5 61
set INCR=-HXC2increaseFac 1
%EXE% -i Bermet..ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 23 -tolR3 23 -tolR4 101 -tolR5 101
set INCR=-HXC2increaseFac 1
%EXE% -i "Bermudadreieck_(Skizze).ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 18 -tolR3 18 -tolR4 80 -tolR5 80
set TOLG=-tolG0 0 -tolG1 0 -tolG2 22 -tolG3 22 -tolG4 86 -tolG5 86
set TOLB=-tolB0 0 -tolB1 0 -tolB2 7 -tolB3 7 -tolB4 69 -tolB5 69
set INCR=-HXC2increaseFac 1
%EXE% -i Bellum_Hiemale_1.ppm -o %OutFile% -histmode none -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
REM %EXE% -i "Bernard_Philippeaux_(1995).ppm" -o %OutFile% -method cobalp2 -st 0	-log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 66 -tolR3 66 -tolR4 99 -tolR5 99
set TOLG=-tolG0 3 -tolG1 3 -tolG2 77 -tolG3 77 -tolG4 77 -tolG5 77
set TOLB=-tolB0 0 -tolB1 0 -tolB2 27 -tolB3 27 -tolB4 59 -tolB5 59
set INCR=-HXC2increaseFac 1
%EXE% -i Bilde_2.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
REM %EXE% -i Blitz_logo_.ppm -o %OutFile% -histmode comp -method cobalp2 -st 0 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 110 -tolR3 110 -tolR4 220 -tolR5 220
set INCR=-HXC2increaseFac 1
%EXE% -i "Boom_(PSF).ppm" -o %OutFile% -histmode none -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i BoxModelingSwanChairLeg4.5.ppm -o %OutFile% -Idx 1 -idx_sorting 1 -histmode none -method TSIP  -skip_rlc1 1 -IFCreset 10 -IFCmax 3 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i Bpmn-3.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i Bulgarians_and_Slavs_VI-VII_century.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 8 -tolR3 8 -tolR4 16 -tolR5 16
set INCR=-HXC2increaseFac 1
%EXE% -i Bpmn-1.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 55 -tolR3 55 -tolR4 77 -tolR5 77
set TOLG=-tolG0 0 -tolG1 0 -tolG2 33 -tolG3 33 -tolG4 33 -tolG5 33
set TOLB=-tolB0 0 -tolB1 0 -tolB2 44 -tolB3 44 -tolB4 44 -tolB5 44
set INCR=-HXC2increaseFac 1
%EXE% -i Byzantium650ADhy.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i C1.ppm -o %OutFile% -Idx 1 -idx_sorting 0 -histmode none -method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 10 -tolR3 10 -tolR4 110 -tolR5 110
set INCR=-HXC2increaseFac 1
%EXE% -i C32_7.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i Carpus.ppm -o %OutFile% -histmode comp -method cobalp2 -sT 0 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 3 -tolR3 3 -tolR4 6 -tolR5 6
set TOLG=-tolG0 0 -tolG1 0 -tolG2 2 -tolG3 2 -tolG4 5 -tolG5 5
set TOLB=-tolB0 0 -tolG1 0 -tolB2 0 -tolB3 0 -tolB4 1 -tolB5 1
set INCR=-HXC2increaseFac 1
%EXE% -i CatBurn-2.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 14 -tolR3 14 -tolR4 17 -tolR5 17
set TOLG=-tolG0 4 -tolG1 4 -tolG2 12 -tolG3 12 -tolG4 12 -tolG5 12
set TOLB=-tolB0 2 -tolB1 2 -tolB2 5 -tolB3 5 -tolB4 11 -tolB5 11
set INCR=-HXC2increaseFac 1
%EXE% -i Cartel_2.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 4 -tolR3 4 -tolR4 6 -tolR5 6
set TOLG=-tolG0 1 -tolG1 1 -tolG2 6 -tolG3 6 -tolG4 14 -tolG5 14
set TOLB=-tolB0 0 -tolB1 0 -tolB2 6 -tolB3 6 -tolB4 7 -tolB5 7
set INCR=-HXC2increaseFac 1
%EXE% -i CatBurn-3.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 10 -tolR3 10 -tolR4 12 -tolR5 12
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1
set TOLB=-tolB0 0 -tolB1 0 -tolB2 1 -tolB3 1 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i Cathedral_schematic_plan_fr.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i "Calvin_cycle_(cs).ppm" -o %OutFile% -histmode none -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i "CellMembraneDrawing_(es).ppm" -o %OutFile% -histmode none -method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 2 -tolR1 2 -tolR2 42 -tolR3 42 -tolR4 58 -tolR5 58
set TOLG=-tolG0 2 -tolG1 2 -tolG2 42 -tolG3 42 -tolG4 64 -tolG5 64
set TOLB=-tolB0 4 -tolB1 4 -tolB2 42 -tolB3 42 -tolB4 64 -tolB5 64
set INCR=-HXC2increaseFac 1
%EXE% -i Cheltenhamproblem..ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
REM %EXE% -i Chansung.ppm -o %OutFile% -histmode comp -method cobalp2 -sT 0 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1
set TOLG=-tolG0 1 -tolG1 1 -tolG2 5 -tolG3 5 -tolG4 6 -tolG5 6
set TOLB=-tolB0 1 -tolB1 1 -tolB2 3 -tolB3 3 -tolB4 3 -tolB5 3
set INCR=-HXC2increaseFac 1
%EXE% -i Chart_of_popularity_of_health-themed_websites_.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 3 -tolR3 3 -tolR4 25 -tolR5 25
set TOLG=-tolG0 0 -tolG1 0 -tolG2 8 -tolG3 8 -tolG4 20 -tolG5 20
set TOLB=-tolB0 4 -tolB1 4 -tolB2 7 -tolB3 7 -tolB4 7 -tolB5 7
set INCR=-HXC2increaseFac 1
%EXE% -i Chloroplasts_-_diffusion_of_photons.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
REM %EXE% -i Cinnamon_1.6.7.ppm -o %OutFile% -histmode comp -method cobalp3 -st 0 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i CicloCombinadoVSdemanda.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 13 -tolR3 13 -tolR4 13 -tolR5 13
set TOLG=-tolG0 0 -tolG1 0 -tolG2 14 -tolG3 14 -tolG4 26 -tolG5 26
set TOLB=-tolB0 0 -tolB1 0 -tolB2 14 -tolB3 14 -tolB4 14 -tolB5 14
set INCR=-HXC2increaseFac 1
%EXE% -i Coatofarmszeeland.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 4 -tolR1 4 -tolR2 12 -tolR3 12 -tolR4 16 -tolR5 16
set TOLG=-tolG0 4 -tolG1 4 -tolG2 12 -tolG3 12 -tolG4 16 -tolG5 16
set TOLB=-tolB0 4 -tolB1 4 -tolB2 12 -tolB3 12 -tolB4 16 -tolB5 16
set INCR=-HXC2increaseFac 1
%EXE% -i Colortest.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i Common-whipping-binding-3.ppm -o %OutFile% -histmode none -method BPC	-log_name %LogFile% >> %res_en%
%EXE% -i Comparison_of_JPEG_and_PNG.ppm -o %OutFile% -Idx 1 -idx_sorting 0 -histmode none -method TSIP  -skip_rlc1 1 -IFCreset 0 -IFCmax 0 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 18 -tolR3 18 -tolR4 255 -tolR5 255
set TOLG=-tolG0 0 -tolG1 0 -tolG2 19 -tolG3 19 -tolG4 255 -tolG5 255
set TOLB=-tolB0 0 -tolB1 0 -tolB2 22 -tolB3 22 -tolB4 255 -tolB5 255
set INCR=-HXC2increaseFac 1
%EXE% -i Compass_Card.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

%EXE% -i Contourgp1.ppm -o %OutFile% -idx 1 -idx_sorting 0 -histmode none -method HXC -tol 0 -toloff1 0 -tolOff2 1 -HXCincreaseFac 2 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i Contourgpns1.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i CopticLetters.ppm -o %OutFile% -histmode none -method BPC -log_name %LogFile% >> %res_en%
%EXE% -i Copolymers.ppm -o %OutFile% -idx 1 -idx_sorting 0 -histmode none -method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -i Corollary_6.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 50 -tolR3 50 -tolR4 100 -tolR5 100
set TOLG=-tolG0 0 -tolG1 0 -tolG2 51 -tolG3 51 -tolG4 65 -tolG5 65
set TOLB=-tolB0 0 -tolB1 0 -tolB2 46 -tolB3 46 -tolB4 94 -tolB5 94
set INCR=-HXC2increaseFac 1
%EXE% -i "Counties_of_Ireland,_sized_according_to_population_(2007).ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 11
%EXE% -i Coupe_.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 2 -tolR3 2 -tolR4 4 -tolR5 4
set TOLG=-tolG0 0 -tolG1 0 -tolG2 6 -tolG3 6 -tolG4 9 -tolG5 9
set TOLB=-tolB0 0 -tolB1 0 -tolB2 2 -tolB3 2 -tolB4 6 -tolB5 6
set INCR=-HXC2increaseFac 1
%EXE% -i "Csite_(2).ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -i "Cuckoo_(PSF).ppm" -o %OutFile% -histmode none -method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 31 -tolR3 31 -tolR4 40 -tolR5 40
set INCR=-HXC2increaseFac 1
%EXE% -i "D3D_Pipeline_(en).ppm" -o %OutFile% -histmode none -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 2 -tolR3 2 -tolR4 3 -tolR5 3
set TOLG=-tolG0 3 -tolG1 3 -tolG2 7 -tolG3 7 -tolG4 11 -tolG5 11
set TOLB=-tolB0 1 -tolB1 1 -tolB2 4 -tolB3 4 -tolB4 4 -tolB5 4
set INCR=-HXC2increaseFac 1
%EXE% -i Denkmal.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
REM %EXE% -i "DeltasABADMensual%.ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
REM %EXE% -i DeskNova1_1_2.ppm -o %OutFile%	-rct A2_2 -histmode none -method BPC -log_name %LogFile% >> %res_en%
%EXE% -i Debian.ppm -o %OutFile% -Idx 1 -idx_sorting 1 -histmode none -method TSIP  -skip_rlc1 1 -IFCreset 10 -IFCmax 3 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 5 -tolR5 5
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 4 -tolG5 4
set TOLB=-tolB0 0 -tolB1 0 -tolB2 1 -tolB3 1 -tolB4 3 -tolB5 3
set INCR=-HXC2increaseFac 1
%EXE% -i Dglogo_2.ppm -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 3 -tolR1 3 -tolR2 16 -tolR3 16 -tolR4 64 -tolR5 64
set TOLG=-tolG0 11 -tolG1 11 -tolG2 25 -tolG3 25 -tolG4 180 -tolG5 180
set TOLB=-tolB0 3 -tolB1 3 -tolB2 17 -tolB3 17 -tolB4 64 -tolB5 64
set INCR=-HXC2increaseFac 1
%EXE% -i Diadochi_2.ppm -o %OutFile% -histmode none -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 2 -tolR3 2 -tolR4 111 -tolR5 111
set TOLG=-tolG0 0 -tolG1 0 -tolG2 2 -tolG3 2 -tolG4 111 -tolG5 111
set TOLB=-tolB0 0 -tolB1 0 -tolB2 2 -tolB3 2 -tolB4 111 -tolB5 111
set INCR=-HXC2increaseFac 1
%EXE% -i "Danke-.ppm" -o %OutFile% -histmode none -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

)



set TOLR=-tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 2 -tolR5 2
set TOLG=-tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 2 -tolG5 2
set TOLB=-tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i "682px-Baden.ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set TOLG=-tolG0 1 -tolG1 1 -tolG2 2 -tolG3 2 -tolG4 2 -tolG5 2
set TOLB=-tolB0 1 -tolB1 1 -tolB2 2 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i "682px-Baden.ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set TOLG=-tolG0 0 -tolG1 0 -tolG2 2 -tolG3 2 -tolG4 2 -tolG5 2
set TOLB=-tolB0 1 -tolB1 1 -tolB2 2 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i "682px-Baden.ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set TOLG=-tolG0 1 -tolG1 1 -tolG2 2 -tolG3 2 -tolG4 2 -tolG5 2
set TOLB=-tolB0 0 -tolB1 0 -tolB2 2 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -i "682px-Baden.ppm" -o %OutFile% -histmode comp -method HXC2 %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%





pause
if !flag! == 1 (

%EXE% -i Diagramm_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Dialecticas2.7.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Dinar_map.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Dobrogea-cadrilater.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Dresden_gemarkungen.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Eco_3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Edinburgh_(location).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Eek'.ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Eiskaltdc++.ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Ejemplo..ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Ek-flow_4.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i electrical.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Embankment_1_(PSF).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Enonteki”.ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Epiphany2.26.1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Excited_states_of_chlorophyll_(cs).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i F.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Favoritlogo_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Fe-55Mn-3Al-3Si_Tensile_properties_-_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Figured_Bass_Inversions_1_and_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Figure_3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Figure_4.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Firefox_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Flag_Arb‰resh‰_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Flag_map_of_Serbia_(without_Kosovo).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Flag_of_Ukraine.svg.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Flussdiagramm_(Programmablaufplan).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Foo_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Forthstack1_5.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Fpp_6-.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Fpp_6.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i France_Israel_Locator_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Friedrich_condenser_(2).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Fromsec4_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Fromsec4_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Galeon_2.0.2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Gallinula_chloropus(PSF).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gamma0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gamma_0_5.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gamma_2_0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gendergap2011.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Geode10.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Geode3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gesch„ftsleitung..ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gocursus-dia1-2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Go_c.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Graf_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gray1120.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gray219.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gray38.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gray577.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Grey_720-360.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Grfsalinidad.3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Gwibber_2.0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Hainaut_Centre.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Haiti_administrative_map-fr.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Haiti_blank_map_with_topography.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Haiti_departements_map-fr.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Haiti_road_map-fr.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Haniielezit.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i HashiwokakeroBeispiel.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "HashiwokakeroL”sung.ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Haxo_(Paris_Metro).ppm"			-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Hene-1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Hene-2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i HeY-.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Hist-gamma4.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Hnl.4.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Homo_lumper_(francais).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Homo_splitter_(deutsch).ppm"			-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Homo_splitter_(english).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Iceberg_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Idh_mundo.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i IE9_Mobile_Acid_3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Igloo0_7.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Illinois_Locator_Map.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Image-Languages-Europe.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i ImperiumRevelation2.0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Inheritance_mixins.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
REM %EXE% -i Inkscape_0.48.1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Inout1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i InstallNova2_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Interacting_Galaxies_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Interacting_Galaxies_3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i IPNA.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Irene_Adler.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i ISN_10020_CSRT_2007_transcript_Pg_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i ISN_10020_CSRT_2007_transcript_Pg_22.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i ISN_10020_CSRT_2007_transcript_Pg_4.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i ISN_200_ARB_2006_transcript_Pg_26.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i isuoef.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Itop1_0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Jaceys-0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Jaceys-2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Joyce2006_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i JULIA3_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Kano1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Kard_Dziwisz_COA.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Karte-DFB-Regionalverb„nde.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Karte_von_Senftenberg.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i KDE_3.5.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i KDE_4.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Keltisch_sprachen.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i KlausWeber-USPatent3518014-Fig1-3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Kriechingen.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i KSpaceDuel1.1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i LaDiscussionScientifiqueDuCorset81_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i LaDiscussionScientifiqueDuCorset93_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Langs_N.Amer.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i LegvEmmaus_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Limitesdeplacastect¢nicas.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i LinkAPix_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i LinkAPix_3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i LinuxLibertine-2.1.0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Localizador-).ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Logo_precise_name.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Logo_Socialize-.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Luxembourg_(he).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i M175_haplogroup_map,_Asia.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Mandelbrot_Leminiscates_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Mapa-1-.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i MapLocalitationMoguer.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i MarkovGraph-1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i MaterialpicD..ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Mazowieckie-administracja.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i MCL_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i MD82v1.0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Me-.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Meandros.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Mesoam‚rica.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Metria.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i MetroTorino.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i MikeOS-2.0.0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Milky_Way_Spiral_Arms.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i MMarmyflag.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i MOLT_7.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
REM %EXE% -i MonogramaBGCP.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Moonalphabet.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i MozillaFirefox3.6.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Name.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Nautilus_3.2.1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i NDsign_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Netzentwuerfe.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
REM %EXE% -i "Niele_Toroni_(1995).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i NikotinviÐtakafl‘Ðirit.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Nodoarbol-bc++.ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Northeast_Caucasian_languages.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Nova.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Nvlado_(cartoon).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Oberschl„sing_01.ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Ode_Bertrand_(2009).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Olivier_Mosset_(2003).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Onu_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Opensuse11_4.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i OpenWrtWR0_9.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Oranienburg_Statuen_Kopie.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Oscilloscope_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Oste014b..ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i OTSUTS_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i P.J..ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Padania.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Panic10.6.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Paso_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Paso_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Paso_3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Periodic_table_(German).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Personalfr†gor..ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Phillips_screwdriver_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Phillips_screw_head_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Photorespiration_cycle_(cs).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Pig...ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Pineapple...ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i PlanoCotoprix..ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i PNG-climate.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i PNGlogoColorLarge_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i PokerTH.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Polen_in_den_Grenzen_vor_1660.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Pontic_steppe_region_around_650_AD.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Porenkowo_Pomorsko_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Portada.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i PotentialSuperpowers.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Previsat3.0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Previsat3_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Prijepolje_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Primera2012-.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Primera2013-.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Pronto-7.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Qfilterimage1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "QLearning+.ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Quest3_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i QuickEK_5.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Quintenzirkeldeluxe.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i R.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Rac01AUS(vertical).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Radhanites.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Radiodrome-simple-y-bw.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Radiodrome-sketch-lettered-colour.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i RadioLublin.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
REM %EXE% -i Railroad1860.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Rangoli....ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Ranking.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Ravenki2.3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Receiver12.2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Receiver12_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Receiver12_3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Rectangle_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Redstar3.0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i RERR.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Riskprem11-1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Riskprem11-4.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i RNaseH-cant_eat.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Romb+.ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i RR5111-0211.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i RR5111-0212.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i RR5111-0220.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i RR5115-0055.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i RR5115-0061.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i RR5514-0066.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Rudder(Flap).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Ruecknahmegrafik.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i RussiaFarEastNumbered.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Russian_checkers_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Sammellinse.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i schriftzeichen.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i ScinetChartDataVisualization.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i SCORE2007_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i ScummVM_1_0_0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Second_World_War_Europe_02_1942_de.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Second_World_War_Europe_11_1939_de.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Senftenberg.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Seth1_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Seth2_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i ShackletonEnduranceCarte.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Shengwei95_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i SHHSLogo.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Short_(finance).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "SignedShortLosslessBug_(PNG).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Sikaku_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i SimilarMotion.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Simple_png.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i SistemamundialdelsigloXIII.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Situacion_Ames.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Spain_(demis).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Splittings-1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Squelch_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Squelch_3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Sshot-1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Sta01AUS(vertical).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Statehood_quarters_map_2009.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Statincomparison_3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Statinfamily.3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Stirling_Beta-Typ.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Stracture_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Supertux0_1_3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Taiwan_road_sign_Art026.2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Taiwan_road_sign_Art027.2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Taiwan_road_sign_Art058.1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "The_Byrds_(1967).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Thracianocree1_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Tmp-1245434728.1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Tmp-1245434728.2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Toboggan_(PSF).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Totem3_2_0.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Tour_de_France_2012_-_Etappe_7.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Tractrix-sketch-lettered-colour.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Transform-.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "TreatyOfSevres_(corrected).ppm" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i TuckerLemmaDiagram.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Tunnel.2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Tunnel.3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i TW-Art031.1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i TW-Art063.2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i TW-Art063.3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i TW-Art068.2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i TW-Art069.6.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i TW-Art080.1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i TW-Art097.1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i TW-Art105.4.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i TW-Art135.2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Ubicaci¢n...ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Ubicaci¢n_C¢rdoba_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Uebersicht-RD.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i UKdo_4.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Umbrello_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Umkehrregel_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Uniteddestinations.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Univert..ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Uridine_to_pseudouridine.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Uvulopalatopharyngoplasty..ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Vegetation.ppm			-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i VELAnalysis_1.3.ppm	-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Vertetimi2.ppm			-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i VMGdwfttw_2.ppm			-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Warship_2.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Waterfall_1.ppm			-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Wavelength.ppm			-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Web3_4_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Weylandknot.ppm			-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i what.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Wikimic2.ppm		-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Wikiminatlas2_0.ppm		-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Wikipic3_1.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Wikisangamolsavam_5.ppm		-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i WiktionaryMasry.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Winboard_4.2.7.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Window_function_(rectangular).ppm"	-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i WOSMWorldmap3.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Wotsk _orlice.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Wpvs_6.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i WritingSystemsOfTheWorld.ppm	-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i X.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i XchatScreenshot2_6_6.ppm	-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "'XXY+X.ppm'" -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Yahya_1_1-5.ppm		-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Yield.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i "Z-scheme_(cs).ppm"	-o %OutFile%	-log_name %LogFile% >> %res_en%
%EXE% -i Zoning2_.ppm -o %OutFile%	-log_name %LogFile% >> %res_en%

pause



echo end of file list

)
 
pause

REM go back to initial directory
cd %currentDir%


pause

REM 
@echo on