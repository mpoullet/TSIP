REM 
 @echo off

setlocal enabledelayedexpansion


REM 
set EXE="C:\Users\Strutz\Documents\C\TSIPcoder_1_34\bin\TSIPcoder_1_34.exe"
REM set EXE="..\bin_debug\TSIPcoder.exe"
set PSNR="PSNR_C.exe"

set OutFile=tmpbSci.tsip
set LogFile=log_TSIP_Best_Sci.txt

REM dir   \B    C:\Users\Strutz\Documents\Daten\CT2\Science\*.png     >> filenames.txt 


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




set res_en="res_test_Science.txt"
set flag=0

REM capture the current directory
set "currentDir=%cd%"

echo %currentDir%

REM change to directory with images
cd C:\Users\Strutz\Documents\Daten\CT2\Science


if !flag! == 1 (
echo
REM these images vompressed best in AUTO mode v1_33
%EXE% -o %OutFile% -i Art-Print-378-Purples-Reds-color-on-white.png -rct Pei09 -method CoBaLP2 -sT 0  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i benzene-homo-glsl-xray1.png -rct A2_3 -method CoBaLP2 -sT 0  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i en-14-ehr-comment-numeric-medical-document.png -rct A7_4 -method CoBaLP2 -sT0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i logo.png -Idx 1 -idx_sorting 0 -histmode none -method BPC -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i nqueens.png -Idx 1 -idx_sorting 0 -histmode none -method HXC -tol 0 -tolOff1 0 -tolOff2 1 -HXCincreasefac 2 -log_name %LogFile% >> %res_en%
REM


%EXE% -o %OutFile% -i 3975590069_7d5e05207e_o.png -rct A7_10 -method CoBaLP2 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i 408px-Killersudoku_color.png -Idx 1 -idx_sorting 1 -method HXC -tol 0 -tolOff1 0 -tolOff2 1 -HXCincreasefac 4 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -o %OutFile% -i 5colors.png -rct 1 -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i 5colors.png -idx 1 -idx_sort 0 -histmode nome -method HXC  -tol 0 -tolOff1 0 -tolOff2 1 -HXCincreasefac 5 -log_name %LogFile% >> %res_en%


%EXE% -o %OutFile% -i A71-Y.png -method CoBaLP2 -sT 1  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 8 -tolR5 8
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 8 -tolG5 8
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 10 -tolB5 10
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i ACTIN.png -rct 1 -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 22 -tolR1 22 -tolR2 22 -tolR3 22 -tolR4 22 -tolR5 22
set TOLG=-tolG0 22 -tolG1 22 -tolG2 22 -tolG3 22 -tolG4 22 -tolG5 22
set TOLB=-tolB0 11 -tolB1 11 -tolB2 11 -tolB3 11 -tolB4 11 -tolB5 11
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i Alvarez_science_path.png -rct hp1 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%


set TOLR=-tolR0 0 -tolR1 0 -tolR2 50 -tolR3 50 -tolR4 110 -tolR5 110
set TOLG=-tolG0 25 -tolG1 25 -tolG2 50 -tolG3 50 -tolG4 110 -tolG5 110
set TOLB=-tolB0 0 -tolB1 0 -tolB2 50 -tolB3 50 -tolB4 110 -tolB5 110
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i b001793kf7.png -rct RGB -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 4 -tolR1 4 -tolR2 5 -tolR3 6 -tolR4 6 -tolR5 6
set TOLG=-tolG0 5 -tolG1 5 -tolG2 5 -tolG3 6 -tolG4 9 -tolG5 6
set TOLB=-tolB0 3 -tolB1 2 -tolB2 2 -tolB3 3 -tolB4 5 -tolB5 3
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i b001793kf8.png -rct RGB -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 4 -tolR1 4 -tolR2 5 -tolR3 6 -tolR4 6 -tolR5 6
set TOLG=-tolG0 5 -tolG1 5 -tolG2 5 -tolG3 6 -tolG4 7 -tolG5 7
set TOLB=-tolB0 2 -tolB1 2 -tolB2 2 -tolB3 3 -tolB4 4 -tolB5 4
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i b001793kf8.png -rct RGB -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%

%EXE% -o %OutFile% -i benzene-homo-glsl-xray-cryos.png  -rct HP1 -method CoBaLP2 -sB 1 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i benzene-homo-glsl-xray-cryos.png  -rct HP1 -method CoBaLP2 -sB 1 -constLines 0 -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 1 -tolR1 1 -tolR2 2 -tolR3 2 -tolR4 2 -tolR5 2
set TOLG=-tolG0 1 -tolG1 1 -tolG2 2 -tolG3 2 -tolG4 2 -tolG5 2
set TOLB=-tolB0 1 -tolB1 1 -tolB2 2 -tolB3 2 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 3
%EXE% -o %OutFile% -i computerized-tomography-us-heatmap.png -rct RGB -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i convert-pdf-to-bmp.png -Idx 1 -idx_sorting 1 -histmode none -method TSIP -skip_rlc1 1 -IFCreset 80 -IFCmax 5 -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i Cosmology_NEP.png  -Idx 1 -idx_sorting 0 -histmode kopt -method TSIP -skip_rlc1 1 -IFCreset 133 -IFCmax 33 -log_name %LogFile% >> %res_en%

%EXE% -o %OutFile% -i cov-journal-science-prev2-med.png -Idx 1 -idx_sorting 1 -histmode none -method TSIP -skip_rlc1 1 -IFCreset 100 -IFCmax 25 -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i F1_medium.png -Idx 1 -idx_sorting 1 -histmode none -method TSIP -skip_rlc1 1 -IFCreset 100 -IFCmax 15 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 2 -tolR1 2 -tolR2 4 -tolR3 4 -tolR4 6 -tolR5 6
set TOLG=-tolG0 2 -tolG1 2 -tolG2 4 -tolG3 4 -tolG4 6 -tolG5 6
set TOLB=-tolB0 2 -tolB1 2 -tolB2 4 -tolB3 4 -tolB4 6 -tolB5 6
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i F6_medium.png -rct RGB -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 7 -tolR1 7 -tolR2 7 -tolR3 16 -tolR4 13 -tolR5 10
set TOLG=-tolG0 9 -tolG1 6 -tolG2 8 -tolG3 14 -tolG4 15 -tolG5 9
set TOLB=-tolB0 4 -tolB1 4 -tolB2 4 -tolB3 10 -tolB4 7 -tolB5 6
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i fig15.png -rct RGB -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 7 -tolR1 7 -tolR2 7 -tolR3 13 -tolR4 13 -tolR5 13
set TOLG=-tolG0 7 -tolG1 7 -tolG2 9 -tolG3 13 -tolG4 13 -tolG5 13
set TOLB=-tolB0 4 -tolB1 4 -tolB2 5 -tolB3 5 -tolB4 7 -tolB5 7
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i fig15.png -rct RGB -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1
set TOLG=-tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 3 -tolG5 3
set TOLB=-tolB0 0 -tolB1 0 -tolB2 1 -tolB3 1 -tolB4 2 -tolB5 2
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i ga-high-10.png -rct RGB -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 23 -tolR1 11 -tolR2 35 -tolR3 25 -tolR4 35 -tolR5 30
set TOLG=-tolG0 16 -tolG1 6 -tolG2 25 -tolG3 6 -tolG4 18 -tolG5 18
set TOLB=-tolB0 26 -tolB1 13 -tolB2 17 -tolB3 21 -tolB4 43 -tolB5 44
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i g_black_hole.png -rct A2_1 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 15 -tolR1 15 -tolR2 30 -tolR3 30 -tolR4 35 -tolR5 35
set TOLG=-tolG0 10 -tolG1 10 -tolG2 15 -tolG3 15 -tolG4 18 -tolG5 18
set TOLB=-tolB0 20 -tolB1 20 -tolB2 17 -tolB3 21 -tolB4 43 -tolB5 44
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i g_black_hole.png -rct A2_1 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

%EXE% -o %OutFile% -i hyades_mosaic.png -idx 1 -idx_sorting 1 -method CoBaLP2 -sA 1 -sB1 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i hyades_mosaic_color.png -idx 1 -idx_sorting 1 -histmode perm -method CoBaLP2 -sA 1 -sB1 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i hyades_mosaic_color.png -idx 1 -idx_sorting 1 -histmode kopt -method CoBaLP2 -sA 1 -sB1 -sT 1 -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i hyades_n.png -Idx 1 -idx_sorting 1 -histmode none -method HXC -tol 0 -tolOff1 60 -tolOff2 100 -HXCincreasefac 1  -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 3
%EXE% -o %OutFile% -i hyades_n.png -rct RGB -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%
set INCR=-HXC2increaseFac 0
%EXE% -o %OutFile% -i hyades_n.png -rct RGB -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR%  -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 64 -tolR3 64 -tolR4 128 -tolR5 128
set TOLG=-tolG0 0 -tolG1 0 -tolG2 64 -tolG3 64 -tolG4 128 -tolG5 128
set TOLB=-tolB0 0 -tolB1 0 -tolB2 64 -tolB3 64 -tolB4 128 -tolB5 128
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i I-love-science1024x768.png -rct RGB -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 3 -tolR3 3 -tolR4 15 -tolR5 15
set TOLG=-tolG0 0 -tolG1 0 -tolG2 3 -tolG3 3 -tolG4 22 -tolG5 22
set TOLB=-tolB0 0 -tolB1 0 -tolB2 3 -tolB3 3 -tolB4 16 -tolB5 16
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i img41.png -rct A2_10 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i inline-graphic-1.png -Idx 1 -idx_sorting 0 -histmode none -method TSIP -skip_rlc1 1 -IFCreset 40 -IFCmax 5 -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 20 -tolR1 20 -tolR2 21 -tolR3 21 -tolR4 22 -tolR5 22
set TOLG=-tolG0 20 -tolG1 20 -tolG2 21 -tolG3 21 -tolG4 22 -tolG5 22
set TOLB=-tolB0 20 -tolB1 20 -tolB2 21 -tolB3 21 -tolB4 22 -tolB5 22
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i InVivoTarget1.png -rct RGB -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i life_science.png -rct B2_1 -histmode comp -method CoBalp2 -sT 0 -log_name %LogFile% >> %res_en%

%EXE% -o %OutFile% -i M0025_kappaxray_error.png -idx 1 -idx_sorting 0 -histmode kopt -method CoBalp2 -sT 0 -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0
set TOLG=-tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0
set TOLB=-tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i mad-scientist-clipart.png -rct RGB -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 5 -tolR3 5 -tolR4 27 -tolR5 27
set TOLG=-tolG0 0 -tolG1 0 -tolG2 5 -tolG3 5 -tolG4 19 -tolG5 19
set TOLB=-tolB0 0 -tolB1 0 -tolB2 3 -tolB3 3 -tolB4 36 -tolB5 036
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i MICFig-121.png -rct A1_7 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i MICFig-122.png -Idx 1 -idx_sorting 0 -histmode none -method TSIP -skip_rlc1 1 -IFCreset 80 -IFCmax 5 -log_name %LogFile% >> %res_en%

set TOLR=-tolR0 0 -tolR1 0 -tolR2 11 -tolR3 11 -tolR4 255 -tolR5 255
set TOLG=-tolG0 0 -tolG1 0 -tolG2 5 -tolG3 5 -tolG4 120 -tolG5 120
set TOLB=-tolB0 0 -tolB1 0 -tolB2 15 -tolB3 15 -tolB4 217 -tolB5 217
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i MITP_USA_2008DEC_tomography_IDV_western_US_2_isosurfaces_planView_300km.png -rct B1_1 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i ncat_fig7_8.png -Idx 1 -idx_sorting 1 -histmode none -method TSIP -skip_rlc1 1 MTF -MTF_val 0 -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i PLOSMapOfScience.png -rct Pei09 -method CoBaLP2 -sT 0 -log_name %LogFile% >> %res_en%

%EXE% -o %OutFile% -i Portal_2_Potatoe_Science_Kit.png -Idx 1 -idx_sorting 1 -histmode none -method TSIP -skip_rlc1 1 -IFCreset 50 -IFCmax 5 -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i Projekt.png -Idx 1 -idx_sorting 0 -histmode none -method BPC -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 5 -tolR1 5 -tolR2 15 -tolR3 15 -tolR4 20 -tolR5 20
set TOLG=-tolG0 5 -tolG1 5 -tolG2 15 -tolG3 15 -tolG4 20 -tolG5 20
set TOLB=-tolB0 5 -tolB1 5 -tolB2 15 -tolB3 15 -tolB4 20 -tolB5 20
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i RXJ0852-46X.png -rct RGB -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 0 -tolR1 0 -tolR2 31 -tolR3 31 -tolR4 63 -tolR5 63
set TOLG=-tolG0 0 -tolG1 0 -tolG2 14 -tolG3 14 -tolG4 74 -tolG5 74
set TOLB=-tolB0 0 -tolB1 0 -tolB2 8 -tolB3 8 -tolB4 25 -tolB5 25
set INCR=-HXC2increaseFac 1
%EXE% -o %OutFile% -i RXJ0852-46_radio.png -rct B2_1 -histmode none -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
)

set TOLR=-tolR0 2 -tolR1 2 -tolR2 5 -tolR3 5 -tolR4 5 -tolR5 5
set TOLG=-tolG0 1 -tolG1 1 -tolG2 4 -tolG3 4 -tolG4 4 -tolG5 4
set TOLB=-tolB0 1 -tolB1 1 -tolB2 4 -tolB3 4 -tolB4 4 -tolB5 4
set INCR=-HXC2increaseFac 0
%EXE% -o %OutFile% -i hyades_mosaic.png -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 5 -tolR3 5 -tolR4 5 -tolR5 5
set TOLG=-tolG0 1 -tolG1 1 -tolG2 4 -tolG3 4 -tolG4 4 -tolG5 4
set TOLB=-tolB0 1 -tolB1 1 -tolB2 4 -tolB3 4 -tolB4 4 -tolB5 4
set INCR=-HXC2increaseFac 0
%EXE% -o %OutFile% -i hyades_mosaic.png -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 4 -tolR3 4 -tolR4 6 -tolR5 6
set TOLG=-tolG0 1 -tolG1 1 -tolG2 4 -tolG3 4 -tolG4 4 -tolG5 4
set TOLB=-tolB0 1 -tolB1 1 -tolB2 4 -tolB3 4 -tolB4 4 -tolB5 4
set INCR=-HXC2increaseFac 0
%EXE% -o %OutFile% -i hyades_mosaic.png -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 4 -tolR3 4 -tolR4 7 -tolR5 7
set TOLG=-tolG0 1 -tolG1 1 -tolG2 4 -tolG3 4 -tolG4 4 -tolG5 4
set TOLB=-tolB0 1 -tolB1 1 -tolB2 4 -tolB3 4 -tolB4 4 -tolB5 4
set INCR=-HXC2increaseFac 0
%EXE% -o %OutFile% -i hyades_mosaic.png -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%
set TOLR=-tolR0 1 -tolR1 1 -tolR2 5 -tolR3 5 -tolR4 6 -tolR5 6
set TOLG=-tolG0 1 -tolG1 1 -tolG2 4 -tolG3 4 -tolG4 4 -tolG5 4
set TOLB=-tolB0 1 -tolB1 1 -tolB2 4 -tolB3 4 -tolB4 4 -tolB5 4
set INCR=-HXC2increaseFac 0
%EXE% -o %OutFile% -i hyades_mosaic.png -histmode comp -method HXC2  %TOLR% %TOLG% %TOLB% %INCR% -log_name %LogFile% >> %res_en%

pause

if !flag! == 1 (
%EXE% -o %OutFile% -i science-fair-project-forum-ad.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i science-papas-team.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i science-tweeps-influence-090422.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i science.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i ScienceWeek2010_03.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i science_process_diagram_big.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i Science_Slam.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i science_teacher.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i science_teacher_2.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i science_thunder.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i Science_Wraps_2010.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i sessa_fig3.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i spree_250.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i ss433_natural_colour.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i stadtplan-california-academy-of-science-1.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i stadtplan-museum-of-science-industry-1.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i SUFig-56.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i SUFig-57.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i TNCCA.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i tutorial001.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i tutorial010.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i Vela_frs.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i Vela_Jr.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i vela_jr_rosat.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i WOBIB_140.png  -log_name %LogFile% >> %res_en%
%EXE% -o %OutFile% -i Yang.png  -log_name %LogFile% >> %res_en%
echo end of file list

)
 


REM go back to initial directory
cd %currentDir%


pause

REM 
@echo on