@echo off

setlocal enabledelayedexpansion

set EXE=TSIPcoder_1_39a.exe
Rem set EXE=..\bin_debug\TSIPcoder.exe
Rem set PSNR=C:\Users\Strutz\Documents\C\TSIPcoder_1_34a\X_Tests\PSNR_C.exe
set OutFile=tmpBTS.tsip
set LogFile=log_TSIP_Best_Set.txt

Rem dir   \B    C:\Users\Strutz\Documents\Daten\CT2\Test_Set\*.ppm     > filenames.txt
Rem dir   \B   C:\Users\Strutz\Documents\Daten\CT2\Test_Set\*.png        >> filenames.txt
Rem dir   \B   C:\Users\Strutz\Documents\Daten\CT2\Test_Set\Not-HXC\*.png        >> filenames.txt
Rem dir   \B   C:\Users\Strutz\Documents\Daten\CT2\Test_Set\Not-HXC\*.ppm        >> filenames.txt

Rem Supported options: \n);
Rem - Help; 1 \n);
Rem  -i input_file (*.tsip, *.png, *.bmp, *.ppm, *pgm, ...)
Rem  -o output_file (*.tsip, *.png, *.bmp, *.ppm, *pgm, ...)
Rem  -rct ( 0..119; colour space )
Rem  -Idx (1 ... use colour palette)
Rem  -Idx_sorting (0 ... Idx_GRB; 1 ... Idx_Lum)
Rem  -method (HXC, TSIP, LOCO, LOCO_T4, CoBaLP2)
Rem  -histMode (comp, none, perm, kopt)
Rem  -pred (none, LEFT, PAETH, MED, MEDPAETH, ADAPT, H26x)
Rem  -tilesize
Rem ## HXC options ##
Rem  -HXCauto (1 ... manual HXC settings will be ignored)
Rem  -tol (0...1; tolerance for HXC coding)
Rem  -tolOff1 (0...10; tolerance offset1 )
Rem  -tolOff2 (0...20; tolerance offset2 )
Rem  -HXCincreasefac (0...100; increase factor)
Rem  ## HXC2 options ##  \n);
Rem  -HXC2auto (1 ... manual HXC2 settings will be ignored) \n);
Rem  -HXC2increasefac (0...100; increase factor) \n);
Rem  -tolR0  \n);
Rem  -tolR1  \n);
Rem  -tolR2  \n);
Rem  -tolR3  \n);
Rem  -tolR4  \n);
Rem  -tolR5  \n);
Rem  -tolG0  \n);
Rem  -tolG1  \n);
Rem  -tolG2  \n);
Rem  -tolG3  \n);
Rem  -tolG4  \n);
Rem  -tolG5  \n);
Rem  -tolB0  \n);
Rem  -tolB1  \n);
Rem  -tolB2  \n);
Rem  -tolB3  \n);
Rem  -tolB4  \n);
Rem  -tolB5  \n);
Rem ## CoBaLP2 options ##
Rem  -sT 1 (skip template matching)
Rem  -sA 1 (skip predictor A)
Rem  -sB 1 (skip predictor B)
Rem  -aR 1 (enable predictor R)
Rem  -constLines 0 (disable constant lines) \n);
Rem ## TSIP options ##
Rem  -segWidth  \n);
Rem  -segHeight \n);
Rem  -interleaving (Y_U_V, YY_uuvv, YY_uvuv, YuvYuv, YYuuvv)
Rem  -skip_rlc1    (0 ... do not skip; 1 ... skip first run-length coding)
Rem  -skip_prec    (0 ... do not skip; 1 ... skip entire pre-coding stage)
Rem  -skip_postBWT (0 ... do not skip; 1 ... skip MTF/IFC)
Rem  -postBWT (MTF, IFC)
Rem  -MTF_val (0 ... 100)
Rem  -IFCmax (1 ... 100)
Rem  -IFCreset (1 ... 10000)
Rem  -RLC2_zeros (1 ... combine zeros only)
Rem  -entropy_coding (0... Huffman; 1 ... arithmetic)
Rem  -separate (0... jointly coding; 1 ... separate coding
Rem ADAPT prediction:
Rem  -blockWidth  (5 - 128)
Rem  -blockHeight (5 - 128)
Rem  -predictors (100000 ... use first predictor)
Rem              (010000 ... use second predictor)
Rem              (110000 ... use two first predictors)
Rem              (101000 ... use first and third predictor)
Rem              (011000 ... use second and third predictor)
Rem              (011111 ... use all predictors but first)


set res_en=res_test_Test_Set1.txt
set flag=0

Rem capture the current directory
set currentDir=%cd%

echo %currentDir%

Rem change to directory with images
Rem cd C:\Users\Strutz\Documents\Daten\CT2\Test_Set

%EXE% -i "73755.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "275000.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "275001.ppm" -o %OutFile% -method CoBaLP2 -rct A4_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "275002.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "275003.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "275004.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "275005.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "275006.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "275007.ppm" -o %OutFile% -method CoBaLP2 -rct A4_7 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "275008.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "275009.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "354000.ppm" -o %OutFile% -method CoBaLP2 -rct A7_11 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "354001.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "354002.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "354003.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "354004.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "354005.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "354006.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "354007.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "354008.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "354009.ppm" -o %OutFile% -method CoBaLP2 -rct A5_7 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "384001.ppm" -o %OutFile% -method CoBaLP2 -rct A7_11 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "384002.ppm" -o %OutFile% -method CoBaLP2 -rct A5_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "384003.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "384004.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode none -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "384006.ppm" -o %OutFile% -method CoBaLP2 -rct A4_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "384007.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "384009.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "384010.ppm" -o %OutFile% -method CoBaLP2 -rct A5_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "384011.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "3047354.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "6061621.ppm" -o %OutFile% -method CoBaLP2 -rct B2_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "!E5$0_1.2.ppm" -o %OutFile% -method LOCO -rct A3_2 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "!E5$0mistermishka.ppm" -o %OutFile% -method LOCO -rct A3_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "$4__(1995).ppm" -o %OutFile% -method LOCO -rct A1_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "_Basalt_bird_.ppm" -o %OutFile% -method LOCO -rct A1_1 -log_name %LogFile% >> %res_en%
%EXE% -i "_Basalt_bird_.ppm" -o %OutFile% -method LOCO -rct A7_12 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "_Basalt_bird_.ppm" -o %OutFile% -method LOCO -rct A9_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "_Basalt_bird_.ppm" -o %OutFile% -method LOCO -rct A8_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "_Carlos_Soto_PNG.ppm" -o %OutFile% -method LOCO -rct PEI09 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "_jrK$08y..ppm" -o %OutFile% -method LOCO -rct A2_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "_vector-glassy-button_grey.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "_vector-glassy-button-cs-by-dragonart.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "_vector-music-melody-preview.ppm" -o %OutFile% -method CoBaLP2 -rct B1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "_vector-music-melody-preview_grey.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "00000n6^hvRg_1.ppm" -o %OutFile% -method LOCO -rct A1_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "00000n6^hvRg_2.ppm" -o %OutFile% -method LOCO -rct A3_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "00000Np_a.ppm" -o %OutFile% -method LOCO -rct RGB -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "01-03_rouders.ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "01-03_rouders_grey.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "01PNG-PNGOUTWin.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -log_name %LogFile% >> %res_en%
%EXE% -i "01PNG-PNGOUTWin.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "02_Schluesselfelder_Schiff-grey.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "02_Schluesselfelder_Schiff-grey_rotated.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "02_Schluesselfelder_Schiff-PNG.ppm" -o %OutFile% -method CoBaLP2 -rct A8_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "02PNG-PNGOUTWin.ppm" -o %OutFile% -method CoBaLP2 -rct A2_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "03PNG-PNGOUTWin.ppm" -o %OutFile% -method CoBaLP2 -rct A2_11 -log_name %LogFile% >> %res_en%
%EXE% -i "03PNG-PNGOUTWin.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "03PNG-PNGOUTWin.ppm" -o %OutFile% -method CoBaLP2 -rct A2_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "03PNG-PNGOUTWin.ppm" -o %OutFile% -method CoBaLP2 -rct A6_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "03PNG-PNGOUTWin.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "06-1986-dibias-science-art-41.ppm" -o %OutFile% -method CoBaLP2 -rct A1_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "1_Earth_(beschriftet).ppm" -o %OutFile% -method LOCO -rct A1_4 -log_name %LogFile% >> %res_en%
%EXE% -i "1_Earth_(beschriftet).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "10]]5@1-2.ppm" -o %OutFile% -method LOCO -rct A3_6 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "10]]5@1-3.ppm" -o %OutFile% -method LOCO -rct A3_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "10]]5@2-2.ppm" -o %OutFile% -method LOCO -rct A2_6 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "1024px-ComputerMemoryHierarchy.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "1024px-Hemispheres_(PSF).ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "1024px-Iceweasel9.0.1.ppm" -o %OutFile% -method BPC -rct A3_1 -histMode none -pred PAETH -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "1024px-Jimbo_and_friends.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "1024px-KanagawaMapCurrent.ppm" -o %OutFile% -method SCFauto -rct A1_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "1024px-Lacoupole.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "1024px-Table-3.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "157K$O]]K9.ppm" -o %OutFile% -method LOCO -rct A2_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "160SignalBdeDUI.ppm" -o %OutFile% -method HXC2 -rct A3_9 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 4 -tolR1 4 -tolR2 17 -tolR3 17 -tolR4 26 -tolR5 26 -tolG0 4 -tolG1 4 -tolG2 26 -tolG3 26 -tolG4 44 -tolG5 44 -tolB0 4 -tolB1 4 -tolB2 9 -tolB3 9 -tolB4 20 -tolB5 20 -log_name %LogFile% >> %res_en%
%EXE% -i "1821_Bandera_Trigarante_AGN.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode perm -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "2_Solar_System_(beschriftet).ppm" -o %OutFile% -method CoBaLP2 -Idx 1 -Idx_sorting 0 -log_name %LogFile% >> %res_en%
%EXE% -i "2_Solar_System_(beschriftet).ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "2_Solar_System_(beschriftet).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "2_Solar_System_(beschriftet).ppm" -o %OutFile% -method HXC2aut -rct A2_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "20_petra_cold_landscape.ppm" -o %OutFile% -method CoBaLP2 -rct A5_5 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "2007.02.01_(png).ppm" -o %OutFile% -method SCFauto -rct A5_5 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "2007.02.01_(png).ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "2007.02.01_(png).ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "2007.02.01_(png).ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "2007.02.01_(png).ppm" -o %OutFile% -method HXC2aut -rct A3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "20080227053020_Myoglobin.ppm" -o %OutFile% -method CoBaLP2 -rct B2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "2175x3075-ortho-green.ppm" -o %OutFile% -method CoBaLP2 -rct B2_1 -log_name %LogFile% >> %res_en%
%EXE% -i "2175x3075-ortho-green.ppm" -o %OutFile% -method CoBaLP2 -rct A2_2 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "248080202_731e632c11_grey.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode none -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "248080202_731e632c11_o.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "267px-Surcoat_(PSF).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 20 -tolR1 20 -tolR2 66 -tolR3 66 -tolR4 111 -tolR5 111 -log_name %LogFile% >> %res_en%
%EXE% -i "29-CGC-RP-2.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "2-Methylpentane_1.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "2mpampjjpg.ppm" -o %OutFile% -method TSIP -rct A3_1 -histMode comp -pred ADAPT -blockWidth 18 -blockHeight Med_Paeth_Lin_GAP_CoBaLP -segWidth 1024 -segHeight 18 -interleaving YY_uvuv -skip_rlc1 0 -postBWT IFC -IFCmax 300 -IFCreset 5000 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "3_Nachbarschaft_der_Sonne_(beschriftet).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -log_name %LogFile% >> %res_en%
%EXE% -i "3_Nachbarschaft_der_Sonne_(beschriftet).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "347px-Talinslstl_(7).ppm" -o %OutFile% -method CoBaLP2 -rct A5_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "384010-2.ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "387px-Microburil_2.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "3975590069_7d5e05207e_o.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "4_Milchstraße_(beschriftet).ppm" -o %OutFile% -method LOCO -rct A2_1 -log_name %LogFile% >> %res_en%
%EXE% -i "4_Milchstraße_(beschriftet).ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "400px-trigramme.ppm" -o %OutFile% -method HXC2 -rct A1_1 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "408px-Killersudoku_color.ppm" -o %OutFile% -method SCFauto -rct A5_10 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "4249587213_e0b0f99bcc_o.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "4506830830_243f2630fb_o.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "454px-Phillips_screwdriver_1.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "491px-NovitatesZoologicae18_Pl03ex.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "492px-Wappen_Heessel_(Burgdorf).ppm" -o %OutFile% -method CoBaLP2 -rct A3_2 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "5#M#B-.ppm" -o %OutFile% -method LOCO -rct A3_2 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "5_Lokale_Gruppe_(beschriftet).ppm" -o %OutFile% -method HXC2 -rct A2_6 -HXC2auto 0 -log_name %LogFile% >> %res_en%
%EXE% -i "5_Lokale_Gruppe_(beschriftet).ppm" -o %OutFile% -method HXC2 -rct A2_6 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "500px-Flag_of_Ukrainian_SSR_(1937-1949).svg.ppm" -o %OutFile% -method HXC2aut -rct A3_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "500px-Roadmap_to_Unicode_BMP_de.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "512px-_Basalt_bird_.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "582px-Pfeffer_Osmotische_Untersuchungen-5-2.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "591px-Urbana2.5.ppm" -o %OutFile% -method CoBaLP2 -rct A7_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "5colors.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "5H1)_.ppm" -o %OutFile% -method LOCO -rct PEI09 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "6_Virgo_Supercluster_(blank).ppm" -o %OutFile% -method LOCO -rct PEI09 -log_name %LogFile% >> %res_en%
%EXE% -i "6_Virgo_Supercluster_(blank).ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "6_Virgo_Supercluster_(blank).ppm" -o %OutFile% -method LOCO -rct A2_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "6_Virgo-Superhaufen_(beschriftet).ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -skip_rlc1 0 -log_name %LogFile% >> %res_en%
%EXE% -i "6_Virgo-Superhaufen_(beschriftet).ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 1477 -skip_rlc1 1 -postBWT IFC -IFCmax 2 -IFCreset 10 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "600px-Flag_of_Ukrainian_SSR.svg.ppm" -o %OutFile% -method HXC2aut -rct A3_8 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "601px-Frangn(2).ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "609px-Victor_(steamboat).ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "613px-Scissors2_(PSF).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 13 -tolR1 13 -tolR2 33 -tolR3 33 -tolR4 33 -tolR5 33 -log_name %LogFile% >> %res_en%
%EXE% -i "615px-Pannier_(PSF).ppm" -o %OutFile% -method BPC -rct 0 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "618px-Speed1c.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "620px-Dynamo_(PSF).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 4 -tolR1 5 -tolR2 7 -tolR3 5 -tolR4 4 -tolR5 5 -log_name %LogFile% >> %res_en%
%EXE% -i "634px-Wiesbaden_districts.ppm" -o %OutFile% -method SCFauto -rct A6_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "640px-Bicycle_rim_diagrams_02_.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "640px-ETO..ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "640px-IJ_(letter).ppm" -o %OutFile% -method BPC -rct 0 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "640px-Pdn3.5.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "640px-SSadditiveblock.ppm" -o %OutFile% -method HXC2 -rct A2_3 -histMode comp -pred MEDPAETH -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 1 -tolG5 1 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "649px-Orna002-Bandmotive.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "649px-Sponge_(PSF).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 20 -tolR1 20 -tolR2 100 -tolR3 100 -tolR4 90 -tolR5 90 -log_name %LogFile% >> %res_en%
%EXE% -i "653px-MieMapCurrent.ppm" -o %OutFile% -method SCFauto -rct A3_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "666px-Tandem_(PSF).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 16 -tolR1 16 -tolR2 55 -tolR3 55 -tolR4 66 -tolR5 66 -log_name %LogFile% >> %res_en%
%EXE% -i "674px-Map_of_south-west_England_shown_within_England_(Met_Office_region).ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "682px-Baden.ppm" -o %OutFile% -method SCFauto -rct A6_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "686px-TSS_2.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "689px-Screens_png.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "719px-FukuokaMapCurrent.ppm" -o %OutFile% -method SCFauto -rct A3_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "726px-Map_of_Old_Great_Bulgaria_version_german_text.svg.ppm" -o %OutFile% -method SCFauto -rct A5_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "733px-GifuMapCurrent.ppm" -o %OutFile% -method SCFauto -rct A6_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "749px-Somerset1899.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "772px-2wdpanc_1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "789px-Fig.1.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "790px-Epiphany3.0.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "793px-Bodhidharma_..ppm" -o %OutFile% -method SCFauto -rct A4_1 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "793px-Bodhidharma_..ppm" -o %OutFile% -method SCFauto -rct A6_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "793px-Bodhidharma_..ppm" -o %OutFile% -method SCFauto -rct A6_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "793px-Bodhidharma_..ppm" -o %OutFile% -method SCFauto -rct A6_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "793px-Fig.2.3.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "795px-Phillips_screw_head_2.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "8_Beobachtbares_Universum_(beschriftet).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -log_name %LogFile% >> %res_en%
%EXE% -i "8_Beobachtbares_Universum_(beschriftet).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "8_Beobachtbares_Universum_(beschriftet).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode none -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-^Pommerit.ppm" -o %OutFile% -method CoBaLP2 -rct A5_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-0849_pilar_ebro_2004.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-C.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode none -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Cesar.ppm" -o %OutFile% -method LOCO -rct A3_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Colgate_(logo).ppm" -o %OutFile% -method BPC -rct B1_2 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-CompizNova2.1.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Cow-2.ppm" -o %OutFile% -method SCFauto -rct A8_5 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Crankshaft_2.ppm" -o %OutFile% -method SCFauto -rct A8_5 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Crankshaft_2.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Crankshaft_2.ppm" -o %OutFile% -method SCFauto -rct A6_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Crankshaft_2.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-CtrlCenterNova1.1.2.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Diap3.1.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-FBReader.0.10.7.ppm" -o %OutFile% -method SCFauto -rct A7_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Fig_4.4.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Gaboautograph.ppm" -o %OutFile% -method CoBaLP2 -rct A9_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Hunnenwanderung.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Lfu4-4.ppm" -o %OutFile% -method CoBaLP2 -rct A6_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-LOM500_1.ppm" -o %OutFile% -method SCFauto -rct A3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-MediaWikiExternalEditorImage.ppm" -o %OutFile% -method SCFauto -rct A6_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Nickerie_(2).ppm" -o %OutFile% -method SCFauto -rct B2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Omul.1.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-PFCS_.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Piazza_(PSF).ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Prinsenhof_(Brugge).ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-SC2222Figure1.1.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Screenshot-1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Solitaire_01.ppm" -o %OutFile% -method CoBaLP2 -rct A9_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Sporskifte_2.ppm" -o %OutFile% -method HXC2 -rct A2_6 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 11 -tolR3 11 -tolR4 80 -tolR5 80 -tolG0 8 -tolG1 8 -tolG2 8 -tolG3 8 -tolG4 8 -tolG5 8 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Spur3_(PSF).ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-SSadditiveblock.ppm" -o %OutFile% -method HXC2aut -rct A2_3 -histMode comp -pred MEDPAETH -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-SvobodaOver5%.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-TEDxPune_Logo_.ppm" -o %OutFile% -method SCFauto -rct A2_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Thorax_(PSF).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 5 -tolR1 5 -tolR2 10 -tolR3 6 -tolR4 5 -tolR5 7 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Wagon_(Combi).ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "800px-Web3.4.1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "815px-Colonne1853+.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "819px-Montenegroetno03.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "857px-Frame_(PSF).ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "889px-Pozi_screw_head_2.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "897px-Reis_3.4.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "899px-DC_neighborhoods_map.ppm" -o %OutFile% -method CoBaLP2 -rct A5_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "960px-Mynd_1.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "960px-The.Matrix.glmatrix.3.ppm" -o %OutFile% -method CoBaLP2 -rct A6_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "A340FAMILY2v1_0.ppm" -o %OutFile% -method HXC2aut -rct 0 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "A340FAMILY2v1_0.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "A340FAMILY2v1_0.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode none -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "A340FAMILY2v1_0.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "A340FAMILY2v1_0.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode none -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "A-4.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "A42.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "A71-Y.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Abadie.jo-Forme-1.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "AbortionLawsMap.ppm" -o %OutFile% -method SCFauto -rct RGB -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "AbortionLawsMap.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Achsensymmetrie.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Acid2Firefox3.0.1.ppm" -o %OutFile% -method SCFauto -rct A3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Acid3detailFirefox3_6.ppm" -o %OutFile% -method SCFauto -rct A3_2 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Acid3detailFirefox3_6.ppm" -o %OutFile% -method SCFauto -rct A6_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Acid3detailFirefox3_6.ppm" -o %OutFile% -method SCFauto -rct A2_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Acid3detailFirefox3_6.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Acid3detailFirefox3_6.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Acid3Fx3_5.ppm" -o %OutFile% -method SCFauto -rct A2_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Acid3iPod1.1.4.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ACTIN.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 8 -tolR5 8 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 8 -tolG5 8 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 10 -tolB5 10 -log_name %LogFile% >> %res_en%
%EXE% -i "Acting...ppm" -o %OutFile% -method HXC2 -rct 0 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 8 -tolR1 8 -tolR2 24 -tolR3 24 -tolR4 24 -tolR5 24 -log_name %LogFile% >> %res_en%
%EXE% -i "Adminer03.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Adminpedia.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Aerocapture_(de).ppm" -o %OutFile% -method HXC2 -rct A2_9 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 10 -tolR3 10 -tolR4 10 -tolR5 10 -tolG0 0 -tolG1 0 -tolG2 9 -tolG3 9 -tolG4 10 -tolG5 10 -tolB0 0 -tolB1 0 -tolB2 10 -tolB3 10 -tolB4 10 -tolB5 10 -log_name %LogFile% >> %res_en%
%EXE% -i "Aerocapture_(de).ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Aerocapture_(de).ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "AgeStructureUa1897+.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "AGV_5.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "AHPHierarchy02_1.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "AHPHierarchy4_0.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Aimags.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Alexandra+Burke+SUPER+HQ++PNG.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Alosizes(v2).ppm" -o %OutFile% -method SCFauto -rct A2_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Alvarez_science_path.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "AlzadosEspartaquistas..ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ambientocclusion-.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "AncienneEgliseSaintDenysDeCoulommiers.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Android-2.3.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Anime_Girl.ppm" -o %OutFile% -method LOCO -rct A2_1 -log_name %LogFile% >> %res_en%
%EXE% -i "Anime_Girl.ppm" -o %OutFile% -method LOCO -rct A2_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Anime_Girl.ppm" -o %OutFile% -method LOCO -rct B3_2 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Anime_Girl.ppm" -o %OutFile% -method LOCO -rct A8_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Anime_Girl.ppm" -o %OutFile% -method LOCO -rct A3_6 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "apple-world-aquagraph-16074.ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ARA.ppm" -o %OutFile% -method SCFauto -rct A1_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Aragon.ppm" -o %OutFile% -method SCFauto -rct A3_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "architecture-5ao.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "architecture-7q7.ppm" -o %OutFile% -method CoBaLP2 -rct A9_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "arial.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode comp -pred none -segWidth 735 -segHeight 493 -skip_rlc1 1 -postBWT IFC -IFCmax 20 -IFCreset 160 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Arsbreytinglandsfr2000-8.ppm" -o %OutFile% -method SCFauto -rct A6_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Arsenal_.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "article-0-0B9771B500000578-590_634x418.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "artifacts-6.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "artificial8_full.ppm" -o %OutFile% -method LOCO -rct A3_11 -log_name %LogFile% >> %res_en%
%EXE% -i "artificial8_full.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Art-Print-378-Purples-Reds-color-on-white.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ashton_landscape.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ashton_landscape_grey.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "aspens.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 703 -skip_rlc1 1 -postBWT IFC -IFCmax 25 -IFCreset 100 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Aurochs1.1.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "auto1.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Avier.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "B.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "B@048F8O_(10).ppm" -o %OutFile% -method LOCO -rct 0 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "b001793kf7.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "b001793kf8.ppm" -o %OutFile% -method SCFauto -rct A3_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "balloon.ppm" -o %OutFile% -method CoBaLP2 -rct A5_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BALLZ.ppm" -o %OutFile% -method SCFauto -rct A3_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Baltic_languages_PL.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BandademúsicaCIGC.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Banner_2.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "banner1-2.ppm" -o %OutFile% -method CoBaLP2 -rct A3_6 -histMode none -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "banner1-3.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "banner2-2.ppm" -o %OutFile% -method CoBaLP2 -rct A2_6 -histMode none -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "barbara.ppm" -o %OutFile% -method TSIP -rct A7_1 -histMode none -pred ADAPT -blockWidth 28 -blockHeight Med_Paeth_Lin_GAP_CoBaLP -segWidth 720 -segHeight 28 -interleaving Y_U_V -skip_rlc1 1 -postBWT IFC -IFCmax 229 -IFCreset 3664 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "barbara.v.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "barbara.y.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "barbara2.ppm" -o %OutFile% -method TSIP -rct A7_11 -histMode none -pred ADAPT -blockWidth 28 -blockHeight Med_Paeth_Lin_GAP_CoBaLP_TM -segWidth 720 -segHeight 28 -interleaving Y_U_V -skip_rlc1 1 -postBWT IFC -IFCmax 229 -IFCreset 3664 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "barbara2.u.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "barbara2.v.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Barnstar_of_High_Culture.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "battery-6rc0.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "beach1.ppm" -o %OutFile% -method CoBaLP2 -rct A5_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "beach2.ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "bear.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BeharQamar.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BeharQamar.ppm" -o %OutFile% -method HXC2aut -rct A3_11 -histMode none -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BeirutMap_.ppm" -o %OutFile% -method SCFauto -rct A3_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bellamy_2.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bellamy_3.ppm" -o %OutFile% -method SCFauto -rct A3_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bellum_Hiemale_1.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Be-map.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "benjerry.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "bentley.ppm" -o %OutFile% -method LOCO -rct RGB -log_name %LogFile% >> %res_en%
%EXE% -i "bentley.ppm" -o %OutFile% -method LOCO -rct A3_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "bentley.ppm" -o %OutFile% -method LOCO -rct A5_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "bentley.ppm" -o %OutFile% -method LOCO -rct A7_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Benzaldehyde.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "benzene-homo-glsl-xray1.ppm" -o %OutFile% -method CoBaLP2 -rct A2_3 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "benzene-homo-glsl-xray-cryos.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 1 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Bermet..ppm" -o %OutFile% -method SCFauto -rct A1_5 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bermudadreieck_(Skizze).ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bernard_Philippeaux_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A8_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Best-Science-Images-2007-II-science-news-653313_461_404.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Best-Science-Images-2007-I-science-news-653287_461_615.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BigBuckBunny-Landscape.ppm" -o %OutFile% -method CoBaLP2 -rct A1_2 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "bike.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A1_2 -log_name %LogFile% >> %res_en%
%EXE% -i "bike.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A2_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "bike.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A4_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "bike.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A2_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "bike.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A6_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "bike3.ppm" -o %OutFile% -method CoBaLP2 -rct RGB -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bild_3.ppm" -o %OutFile% -method CoBaLP2 -rct A3_2 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bilde_2.ppm" -o %OutFile% -method SCFauto -rct A5_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Biserica_Buna_Vestire,_painting_1.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Biserica_Buna_Vestire,_painting_2.ppm" -o %OutFile% -method LOCO -rct A2_1 -log_name %LogFile% >> %res_en%
%EXE% -i "Biserica_Buna_Vestire,_painting_2.ppm" -o %OutFile% -method LOCO -rct A7_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Biserica_Buna_Vestire,_painting_2.ppm" -o %OutFile% -method LOCO -rct A4_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Biserica_Buna_Vestire,_painting_2.ppm" -o %OutFile% -method LOCO -rct PEI09 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Biserica_Buna_Vestire,_painting_3.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bixio_2.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode none -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "black.ppm" -o %OutFile% -method TSIP -rct A7_4 -histMode comp -pred ADAPT -blockWidth 28 -blockHeight Med_Paeth_Lin_GAP_CoBaLP -segWidth 720 -segHeight 28 -interleaving YY_uuvv -skip_rlc1 0 -postBWT IFC -IFCmax 229 -IFCreset 3664 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "black.u.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "black.y.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Blanco1_0.ppm" -o %OutFile% -method TSIP -rct A3_7 -skip_rlc1 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Blanco1_0.ppm" -o %OutFile% -method TSIP -rct A3_7 -histMode comp -pred ADAPT -blockWidth 35 -blockHeight Med_Paeth_Lin_GAP_CoBaLP -segWidth 1417 -segHeight 35 -interleaving Y_U_V -skip_rlc1 0 -postBWT IFC -IFCmax 315 -IFCreset 5040 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Blason78.ppm" -o %OutFile% -method CoBaLP2 -rct A3_9 -histMode none -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Blitz_logo_.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BlockMap_Photoshop.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -log_name %LogFile% >> %res_en%
%EXE% -i "BlockMap_Photoshop.ppm" -o %OutFile% -method CoBaLP2 -rct A8_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BlockMap_Photoshop.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Blood-Sucking-Moths-science-news-2696550-468-258.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BML_CAT_Chartres_03k.ppm" -o %OutFile% -method HXC2 -rct A3_11 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 255 -tolR3 255 -tolR4 255 -tolR5 255 -tolG0 2 -tolG1 2 -tolG2 444 -tolG3 444 -tolG4 444 -tolG5 444 -tolB0 17 -tolB1 14 -tolB2 228 -tolB3 228 -tolB4 228 -tolB5 228 -log_name %LogFile% >> %res_en%
%EXE% -i "boats.u.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "boats.y.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BodhiLinux2_1_0.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bonniersuppslagsbok.ppm" -o %OutFile% -method CoBaLP2 -rct A9_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bonniersuppslagsbok-.ppm" -o %OutFile% -method CoBaLP2 -rct A9_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "books.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Boom_(PSF).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 110 -tolR3 110 -tolR4 220 -tolR5 220 -log_name %LogFile% >> %res_en%
%EXE% -i "boots.ppm" -o %OutFile% -method TSIP -rct A2_4 -histMode none -pred MEDPAETH -segWidth 1018 -interleaving YY_uvuv -skip_rlc1 0 -postBWT IFC -IFCmax 273 -IFCreset 4368 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BoxModelingSwanChairLeg4.5.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 579 -skip_rlc1 1 -postBWT IFC -IFCmax 3 -IFCreset 10 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bpmn-1.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bpmn-3.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bretagne2.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bretagne2.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bretagne2.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BriarcliffLawPark2013-3.ppm" -o %OutFile% -method LOCO -rct A7_4 -log_name %LogFile% >> %res_en%
%EXE% -i "BriarcliffLawPark2013-3.ppm" -o %OutFile% -method LOCO -rct B3_2 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "BriarcliffLawPark2013-3.ppm" -o %OutFile% -method LOCO -rct A3_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "BriarcliffLawPark2013-3.ppm" -o %OutFile% -method LOCO -rct A3_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "BriarcliffLawPark2013-3.ppm" -o %OutFile% -method LOCO -rct A3_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "BriarcliffLawPark2013-3.ppm" -o %OutFile% -method LOCO -rct A3_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "BriarcliffLawPark2013-3.ppm" -o %OutFile% -method LOCO -rct A3_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "BriarcliffLawPark2013-3.ppm" -o %OutFile% -method LOCO -rct A3_5 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "brick.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "brick.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "brick.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "brick.ppm" -o %OutFile% -method CoBaLP2 -rct A2_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "brick.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bubblegum_3'_(by_Marendo_Müller).ppm" -o %OutFile% -method LOCO -rct A8_1 -log_name %LogFile% >> %res_en%
%EXE% -i "Bubblegum_3'_(by_Marendo_Müller).ppm" -o %OutFile% -method LOCO -rct A3_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Bubblegum_3'_(by_Marendo_Müller).ppm" -o %OutFile% -method LOCO -rct B4_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Bubblegum_3'_(by_Marendo_Müller).ppm" -o %OutFile% -method LOCO -rct PEI09 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Bubblegum_3'_(by_Marendo_Müller).ppm" -o %OutFile% -method LOCO -rct RGB -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Bubblegum_3'_(by_Marendo_Müller).ppm" -o %OutFile% -method LOCO -rct A3_8 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Bubblegum_3'_(by_Marendo_Müller).ppm" -o %OutFile% -method LOCO -rct B1_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Bubblegum_3'_(by_Marendo_Müller).ppm" -o %OutFile% -method LOCO -rct PEI09 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Bubblegum_3'_(by_Marendo_Müller).ppm" -o %OutFile% -method LOCO -rct RGB -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Bulgarians_and_Slavs_VI-VII_century.ppm" -o %OutFile% -method HXC2 -rct A8_1 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "bullfight.ppm" -o %OutFile% -method CoBaLP2 -rct A4_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BumbleBee_HedKase.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode none -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "BumbleBee_HedKase.ppm" -o %OutFile% -method HXC2aut -rct B2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BumbleBee_HedKase.ppm" -o %OutFile% -method HXC2aut -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BumbleBee_HedKase.ppm" -o %OutFile% -method HXC2aut -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BumbleBee_HedKase.ppm" -o %OutFile% -method HXC2aut -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BumbleBee_HedKase.ppm" -o %OutFile% -method HXC2aut -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BumbleBee_HedKase.ppm" -o %OutFile% -method HXC2aut -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BumbleBee_HedKase.ppm" -o %OutFile% -method HXC2aut -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BumbleBee_HedKase.ppm" -o %OutFile% -method HXC2aut -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "BwKrhs_Hamburg_(V3).ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Byzantium650ADhy.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "C.T..ppm" -o %OutFile% -method HXC2aut -rct A6_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "C0017942-Normal_brain,_3D_CT_scan-SPL.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "C1.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "C32_7.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 11 -tolR3 11 -tolR4 101 -tolR5 101 -log_name %LogFile% >> %res_en%
%EXE% -i "cafe.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "cafe.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A3_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cafe.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A6_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cafe.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cafe.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A6_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cajamarquilla_1.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Calvin_cycle_(cs).ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Capa..ppm" -o %OutFile% -method CoBaLP2 -rct A3_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Capilla_Cand.9.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "captured_Image-png_4d60c4b0-7133-4ef8-bbe0-be0e27b0bf29.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "CarolineDeHaas.ppm" -o %OutFile% -method CoBaLP2 -rct A7_12 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "carpets-j4h.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Carpus.ppm" -o %OutFile% -method CoBaLP2 -rct A6_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Carte_des_transports_en_communs_entre_les_bibliothèques_de_Rennes.ppm" -o %OutFile% -method CoBaLP2 -rct A2_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Carte_des_transports_en_communs_entre_les_bibliothèques_de_Rennes.ppm" -o %OutFile% -method HXC2aut -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cartel_2.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "CatBurn-2.ppm" -o %OutFile% -method SCFauto -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "CatBurn-3.ppm" -o %OutFile% -method SCFauto -rct PEI09 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cathedral.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cathedral_schematic_plan_fr.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cathedral.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cathedral.ppm" -o %OutFile% -method CoBaLP2 -rct A1_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cathedral.ppm" -o %OutFile% -method CoBaLP2 -rct B1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cave02.ppm" -o %OutFile% -method CoBaLP2 -rct A2_3 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "CellMembraneDrawing_(es).ppm" -o %OutFile% -method BPC -rct A3_2 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "cement.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cevennes1.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -log_name %LogFile% >> %res_en%
%EXE% -i "Cevennes1.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cevennes1.ppm" -o %OutFile% -method CoBaLP2 -rct A9_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cevennes1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cevennes2.ppm" -o %OutFile% -method CoBaLP2 -rct A5_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Chacchoben_2.ppm" -o %OutFile% -method CoBaLP2 -rct A5_5 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Chansung.ppm" -o %OutFile% -method CoBaLP2 -rct A9_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "chapm-natphot-sug2.ppm" -o %OutFile% -method BPC -rct A2_5 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "chapm-natphot-sug2.ppm" -o %OutFile% -method CoBaLP2 -rct A2_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "chapm-natphot-sug2.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "chapm-natphot-sug2.ppm" -o %OutFile% -method CoBaLP2 -rct A1_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "chapm-natphot-sug2.ppm" -o %OutFile% -method BPC -rct A2_5 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Chart_of_popularity_of_health-themed_websites_.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Chart_pulinco.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "chart2.ppm" -o %OutFile% -method HXC2aut -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ChartBuilder.ppm" -o %OutFile% -method HXC2 -rct A2_11 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Cheltenhamproblem..ppm" -o %OutFile% -method HXC2 -rct A3_4 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 2 -tolR1 2 -tolR2 42 -tolR3 42 -tolR4 58 -tolR5 58 -tolG0 2 -tolG1 2 -tolG2 42 -tolG3 42 -tolG4 64 -tolG5 64 -tolB0 4 -tolB1 4 -tolB2 42 -tolB3 42 -tolB4 64 -tolB5 64 -log_name %LogFile% >> %res_en%
%EXE% -i "Cheltenhamproblem..ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cheltenhamproblem..ppm" -o %OutFile% -method SCFauto -rct A2_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "chinook.ppm" -o %OutFile% -method CoBaLP2 -rct A2_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Chloroplasts_-_diffusion_of_photons.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "CicloCombinadoVSdemanda.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "CIExy1931.ppm" -o %OutFile% -method CoBaLP2 -rct A2_2 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cinnamon_1.6.7.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "circuit-board.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ciudad-2_0.ppm" -o %OutFile% -method HXC2aut -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "clockwork-4.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cmpndd.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode kopt -pred none -segWidth 768 -skip_rlc1 1 -postBWT IFC -IFCmax 7 -IFCreset 92 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cmpndn.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Coatofarmszeeland.ppm" -o %OutFile% -method HXC2 -rct B3_2 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 2 -tolR1 2 -tolR2 13 -tolR3 13 -tolR4 13 -tolR5 13 -tolG0 1 -tolG1 1 -tolG2 14 -tolG3 14 -tolG4 26 -tolG5 26 -tolB0 1 -tolB1 1 -tolB2 14 -tolB3 14 -tolB4 14 -tolB5 14 -log_name %LogFile% >> %res_en%
%EXE% -i "coffee2-216x300.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Colette_Deblé_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Collage_-_portada_5.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Collagecinemaitaliano.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Colortest.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 4 -tolR1 4 -tolR2 12 -tolR3 12 -tolR4 16 -tolR5 16 -tolG0 4 -tolG1 4 -tolG2 12 -tolG3 12 -tolG4 16 -tolG5 16 -tolB0 4 -tolB1 4 -tolB2 12 -tolB3 12 -tolB4 16 -tolB5 16 -log_name %LogFile% >> %res_en%
%EXE% -i "colours-10.ppm" -o %OutFile% -method CoBaLP2 -rct A7_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "colours-an5.ppm" -o %OutFile% -method CoBaLP2 -rct A2_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Common-whipping-binding-3.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Comparison_of_JPEG_and_PNG.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode none -pred none -segWidth 200 -skip_rlc1 1 -postBWT IFC -IFCmax 0 -IFCreset 0 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Compass_Card.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "computer_science2.ppm" -o %OutFile% -method CoBaLP2 -rct RGB -log_name %LogFile% >> %res_en%
%EXE% -i "computer_science2.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "computer_science2.ppm" -o %OutFile% -method CoBaLP2 -rct A3_3 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "computer_science2.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "computer_science2.ppm" -o %OutFile% -method CoBaLP2 -rct A3_3 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "computer-301.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "computer-309.ppm" -o %OutFile% -method CoBaLP2 -rct A3_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "computerized-tomography-us-heatmap.ppm" -o %OutFile% -method SCFauto -rct B2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "computer-science-ultimate.ppm" -o %OutFile% -method CoBaLP2 -rct A1_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cone-response.ppm" -o %OutFile% -method HXC2aut -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "construction-06.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "construction-g6vr.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "construction-w7xq.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Contourgp1.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Contourgpns1.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "convert-pdf-to-bmp.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Copolymers.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "CopticLetters.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "corn-j7z2.ppm" -o %OutFile% -method CoBaLP2 -rct A6_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Corollary_6.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cosmology_NEP.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode kopt -pred none -segWidth 580 -skip_rlc1 0 -postBWT IFC -IFCmax 33 -IFCreset 133 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Counties_of_Ireland,_sized_according_to_population_(2007).ppm" -o %OutFile% -method SCFauto -rct B2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Coupe_.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "couple.ppm" -o %OutFile% -method CoBaLP2 -rct B1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cover07_img10.ppm" -o %OutFile% -method CoBaLP2 -rct A2_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cov-journal-science-prev2-med.ppm" -o %OutFile% -method SCFauto -rct A2_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "CracinusMont.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -log_name %LogFile% >> %res_en%
%EXE% -i "CracinusMont.ppm" -o %OutFile% -method CoBaLP2 -rct A8_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "CracinusMont.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cristo_1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "crowd-people-g3ky.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Csite_(2).ppm" -o %OutFile% -method SCFauto -rct A7_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "CuartelSantaCatalina1934.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "cubeiccp.ppm" -o %OutFile% -method TSIP -rct RGB -histMode comp -pred none -segWidth 600 -interleaving YYuuvv -skip_rlc1 1 -postBWT MTF -MTF_val 0 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Cuckoo_(PSF).ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "cwheel.ppm" -o %OutFile% -method CoBaLP2 -rct RGB -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Cxemamistermishka.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "D_Z.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "D3D_Pipeline_(en).ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Dani.ppm" -o %OutFile% -method CoBaLP2 -rct A5_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Daniel_Humair_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A2_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Danke-.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "DarkWorld.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode none -pred none -segWidth 100000 -skip_rlc1 1 -postBWT IFC -IFCmax 3 -IFCreset 8 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "DarkWorld.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode none -pred none -segWidth 2048 -skip_rlc1 1 -postBWT IFC -IFCmax 3 -IFCreset 8 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "dc7b126a-20f9-4569-bfd4-ce10defa335d_1.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Debian.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 1600 -skip_rlc1 1 -postBWT IFC -IFCmax 3 -IFCreset 10 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "DeltasABADMensual%.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "demos_explorer.ppm" -o %OutFile% -method CoBaLP2 -rct A1_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "demos_gift-demo.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Denkmal.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 2 -tolR3 2 -tolR4 3 -tolR5 3 -tolG0 3 -tolG1 3 -tolG2 7 -tolG3 7 -tolG4 11 -tolG5 11 -tolB0 1 -tolB1 1 -tolB2 4 -tolB3 4 -tolB4 4 -tolB5 4 -log_name %LogFile% >> %res_en%
%EXE% -i "Desert_landscape.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Desert_landscape_grey.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "DeskNova1_1_2.ppm" -o %OutFile% -method BPC -rct A2_2 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DeskNova1_1_2_1024x768.ppm" -o %OutFile% -method BPC -rct A2_5 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DeskNova1_1_2_1024x768.ppm" -o %OutFile% -method BPC -rct B2_3 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DeskNova1_1_2_1024x768.ppm" -o %OutFile% -method BPC -rct A2_2 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DeskNova1_1_2_1024x768.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8p120.ppm" -o %OutFile% -method BPC -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8p120.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8p134.ppm" -o %OutFile% -method BPC -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8p134.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8pVIIIa.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8pVIIIa.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8pX.ppm" -o %OutFile% -method BPC -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8pX.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8pXII.ppm" -o %OutFile% -method SCFauto -rct 0 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8pXII.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "DessousElegants1905n8pXII.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "detroit20science20center-full.ppm" -o %OutFile% -method CoBaLP2 -rct A1_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Deutschland_Übersichtskarte.ppm" -o %OutFile% -method CoBaLP2 -rct A9_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Deutschland_Übersichtskarte.ppm" -o %OutFile% -method LOCO -rct A1_7 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Deutschland_Übersichtskarte.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "dev.ppm" -o %OutFile% -method HXC2 -rct A2_11 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 222 -tolR5 222 -tolG0 0 -tolG1 0 -tolG2 5 -tolG3 5 -tolG4 222 -tolG5 222 -tolB0 0 -tolB1 0 -tolB2 5 -tolB3 5 -tolB4 222 -tolB5 222 -log_name %LogFile% >> %res_en%
%EXE% -i "Dewali_2.ppm" -o %OutFile% -method HXC2 -rct A2_5 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 19 -tolR3 19 -tolR4 23 -tolR5 23 -tolG0 5 -tolG1 5 -tolG2 92 -tolG3 92 -tolG4 92 -tolG5 92 -tolB0 0 -tolB1 0 -tolB2 14 -tolB3 14 -tolB4 46 -tolB5 46 -log_name %LogFile% >> %res_en%
%EXE% -i "Dewali_5.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 1 -histMode perm -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Dglogo_2.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "diabolik_001.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "diabolik_002.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Diabolik_animato.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Diadochi_2.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Diagramm_1.ppm" -o %OutFile% -method SCFauto -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Diagramm_1.2.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Dialecticas2.7.ppm" -o %OutFile% -method CoBaLP2 -rct A8_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "DibuFio.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 1283 -skip_rlc1 0 -postBWT IFC -IFCmax 33 -IFCreset 111 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "didle.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Die_Han-Dynastie_Ua_Eurasia_Situation_(SR).ppm" -o %OutFile% -method SCFauto -rct A8_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "die_Umlaufbahn.ppm" -o %OutFile% -method SCFauto -rct A8_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Dinar_map.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "disneyBMP.ppm" -o %OutFile% -method CoBaLP2 -rct A6_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Dobrogea-cadrilater.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "door-e5f8.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "DR1937_1.ppm" -o %OutFile% -method LOCO -rct A4_1 -log_name %LogFile% >> %res_en%
%EXE% -i "DR1937_1.ppm" -o %OutFile% -method LOCO -rct A8_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "DR1937_1.ppm" -o %OutFile% -method LOCO -rct A2_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "DR1937_1.ppm" -o %OutFile% -method LOCO -rct A3_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "DR1937_1.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Draagkabel_(bovenleiding).ppm" -o %OutFile% -method CoBaLP2 -rct A9_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Dramacentre_.ppm" -o %OutFile% -method CoBaLP2 -rct A6_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Dresden_gemarkungen.ppm" -o %OutFile% -method BPC -rct 0 -histMode perm -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "dumper.ppm" -o %OutFile% -method SCFauto -rct A1_10 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "E.S.B..ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "eagle.ppm" -o %OutFile% -method CoBaLP2 -rct A7_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ecb_berlin_07t.ppm" -o %OutFile% -method HXC2 -rct A8_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 255 -tolR3 255 -tolR4 255 -tolR5 255 -tolG0 2 -tolG1 2 -tolG2 444 -tolG3 444 -tolG4 444 -tolG5 444 -tolB0 17 -tolB1 14 -tolB2 228 -tolB3 228 -tolB4 228 -tolB5 228 -log_name %LogFile% >> %res_en%
%EXE% -i "ecb_ny_89t.ppm" -o %OutFile% -method HXC2 -rct A2_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ecb-assembler-17t-1.ppm" -o %OutFile% -method HXC2aut -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Eclairage.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Eclairage.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Eco_3.ppm" -o %OutFile% -method SCFauto -rct A1_10 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Edinburgh_(location).ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 236 -segHeight Inf1_0 -skip_rlc1 0 -postBWT IFC -IFCmax 5 -IFCreset 80 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Eek'.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 4 -tolR1 4 -tolR2 64 -tolR3 64 -tolR4 64 -tolR5 64 -tolG0 4 -tolG1 4 -tolG2 64 -tolG3 64 -tolG4 64 -tolG5 64 -tolB0 4 -tolB1 4 -tolB2 64 -tolB3 64 -tolB4 64 -tolB5 64 -log_name %LogFile% >> %res_en%
%EXE% -i "EightTNOs.ppm" -o %OutFile% -method CoBaLP2 -rct A2_10 -log_name %LogFile% >> %res_en%
%EXE% -i "EightTNOs.ppm" -o %OutFile% -method CoBaLP2 -rct A2_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Eiskaltdc++.ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ejemplo..ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ek-flow_4.ppm" -o %OutFile% -method HXC2 -rct A2_1 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 128 -tolR3 128 -tolR4 128 -tolR5 128 -tolG0 0 -tolG1 0 -tolG2 119 -tolG3 119 -tolG4 119 -tolG5 119 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "electrical.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Eluanbi_.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Embankment_1_(PSF).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 110 -tolR3 110 -tolR4 220 -tolR5 220 -log_name %LogFile% >> %res_en%
%EXE% -i "EMBLEMADEGORRAGC.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "EMBLEMADEGORRAPSGC.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "en-14-ehr-comment-numeric-medical-document.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Enontekiö.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Epiphany2.26.1.ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "EPIRB@.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Esja_Şverfellshorn_panoramic_view_of_Reykjavík.ppm" -o %OutFile% -method LOCO -rct A4_4 -log_name %LogFile% >> %res_en%
%EXE% -i "Esja_Şverfellshorn_panoramic_view_of_Reykjavík.ppm" -o %OutFile% -method LOCO -rct A7_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Esja_Şverfellshorn_panoramic_view_of_Reykjavík.ppm" -o %OutFile% -method LOCO -rct A4_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Esja_Şverfellshorn_panoramic_view_of_Reykjavík.ppm" -o %OutFile% -method LOCO -rct A7_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Esja_Şverfellshorn_panoramic_view_of_Reykjavík.ppm" -o %OutFile% -method LOCO -rct A9_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "europa_karte_de.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 110 -tolR5 110 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 110 -tolG5 110 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 110 -tolB5 110 -log_name %LogFile% >> %res_en%
%EXE% -i "Europe_regions_(uk).ppm" -o %OutFile% -method HXC2aut -rct RGB -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Europe_regions_(uk).ppm" -o %OutFile% -method HXC2aut -rct A7_10 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Europe_regions_(uk).ppm" -o %OutFile% -method HXC2aut -rct A7_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Excited_states_of_chlorophyll_(cs).ppm" -o %OutFile% -method SCF -rct A2_3 -histMode comp -pred none -HXC2auto 0 -SCF_prediction 1 -SCF_directional 1 -SCF_stage2tuning 1 -SCF_colourMAP 1 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Exemplo_4.ppm" -o %OutFile% -method CoBaLP2 -rct A7_12 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "F.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "F1_large.ppm" -o %OutFile% -method CoBaLP2 -rct A1_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "F1_medium.ppm" -o %OutFile% -method HXC2 -rct A3_1 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 2 -tolR1 2 -tolR2 4 -tolR3 4 -tolR4 6 -tolR5 6 -tolG0 2 -tolG1 2 -tolG2 4 -tolG3 4 -tolG4 6 -tolG5 6 -tolB0 2 -tolB1 2 -tolB2 4 -tolB3 4 -tolB4 6 -tolB5 6 -log_name %LogFile% >> %res_en%
%EXE% -i "F6_medium.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 2 -tolR1 2 -tolR2 4 -tolR3 4 -tolR4 6 -tolR5 6 -tolG0 2 -tolG1 2 -tolG2 4 -tolG3 4 -tolG4 6 -tolG5 6 -tolB0 2 -tolB1 2 -tolB2 4 -tolB3 4 -tolB4 6 -tolB5 6 -log_name %LogFile% >> %res_en%
%EXE% -i "Facade.ppm" -o %OutFile% -method CoBaLP2 -rct RGB -log_name %LogFile% >> %res_en%
%EXE% -i "Facade.ppm" -o %OutFile% -method CoBaLP2 -rct A6_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Facade.ppm" -o %OutFile% -method CoBaLP2 -rct A6_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "face1.ppm" -o %OutFile% -method CoBaLP2 -rct B1_2 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Farbkreis_Itten_1961.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "favicons_large.ppm" -o %OutFile% -method CoBaLP2 -rct A8_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Favoritlogo_1.ppm" -o %OutFile% -method SCFauto -rct A1_5 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Fe_55Mn_3Al_3Si_Tensile_properties_-_1.ppm" -o %OutFile% -method SCFauto -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Felice_Varini_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A8_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FErsBtl_856_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct A6_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "fig15.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "fig2_howe.pg47.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Figure_3.ppm" -o %OutFile% -method SCFauto -rct A2_5 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Figure_4.ppm" -o %OutFile% -method SCFauto -rct A2_5 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Figured_Bass_Inversions_1_and_2.ppm" -o %OutFile% -method BPC -rct 0 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "filou.ppm" -o %OutFile% -method CoBaLP2 -Idx 1 -Idx_sorting 0 -histMode kopt -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "finger.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Firefox_.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A3_1 -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_9 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_9 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_9 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "First_colored_senator_and_reps.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Fishbourne_model_2.ppm" -o %OutFile% -method CoBaLP2 -rct A5_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FJgBtl_610_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Flag_Arbëreshë_.ppm" -o %OutFile% -method HXC2aut -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Flag_map_of_Serbia_(without_Kosovo).ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Flag_of_Ukraine.svg.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred MEDPAETH -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "flaggen_580x2500.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "flax.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Flexicarr-TM.ppm" -o %OutFile% -method CoBaLP2 -rct A5_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "flowers_art.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "flowers-k5cq.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "flowers-y02o.ppm" -o %OutFile% -method CoBaLP2 -rct A8_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Flussdiagramm_(Programmablaufplan).ppm" -o %OutFile% -method SCFauto -rct A1_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FNZg_7.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "fonctionnalites-09-en-h.ppm" -o %OutFile% -method HXC2aut -rct A1_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Foo_2.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 14 -tolR3 14 -tolR4 14 -tolR5 14 -tolG0 1 -tolG1 1 -tolG2 13 -tolG3 13 -tolG4 13 -tolG5 13 -tolB0 1 -tolB1 1 -tolB2 13 -tolB3 13 -tolB4 13 -tolB5 13 -log_name %LogFile% >> %res_en%
%EXE% -i "Formel_2.ppm" -o %OutFile% -method HXC2 -rct A1_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 81 -tolR3 81 -tolR4 90 -tolR5 90 -tolG0 0 -tolG1 0 -tolG2 72 -tolG3 72 -tolG4 110 -tolG5 110 -tolB0 0 -tolB1 0 -tolB2 37 -tolB3 37 -tolB4 79 -tolB5 79 -log_name %LogFile% >> %res_en%
%EXE% -i "Forthstack1_5.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 8 -tolR1 8 -tolR2 64 -tolR3 64 -tolR4 128 -tolR5 128 -log_name %LogFile% >> %res_en%
%EXE% -i "Fpp_6.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Fpp_6-.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "fractal.ppm" -o %OutFile% -method SCFauto -rct A2_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "fragerance1.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FRANCE_-_Caucasus_ethnic.ppm" -o %OutFile% -method HXC2aut -rct A2_4 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FRANCE_-_Caucasus_ethnic.ppm" -o %OutFile% -method HXC2aut -rct A2_5 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FRANCE_-_Caucasus_ethnic.ppm" -o %OutFile% -method HXC2aut -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FRANCE_-_Caucasus_ethnic.ppm" -o %OutFile% -method HXC2aut -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "France_Israel_Locator_2.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 4 -tolR1 4 -tolR2 20 -tolR3 20 -tolR4 20 -tolR5 20 -tolG0 5 -tolG1 5 -tolG2 16 -tolG3 16 -tolG4 21 -tolG5 21 -tolB0 5 -tolB1 5 -tolB2 16 -tolB3 16 -tolB4 21 -tolB5 21 -log_name %LogFile% >> %res_en%
%EXE% -i "FRD-Bonn-20s-882x623.ppm" -o %OutFile% -method HXC2 -rct A2_11 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 21 -tolR1 14 -tolR2 22 -tolR3 29 -tolR4 36 -tolR5 31 -tolG0 18 -tolG1 9 -tolG2 14 -tolG3 19 -tolG4 23 -tolG5 19 -tolB0 7 -tolB1 2 -tolB2 6 -tolB3 6 -tolB4 7 -tolB5 6 -log_name %LogFile% >> %res_en%
%EXE% -i "Fred_Deux_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A2_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "-Fresco-.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Friedrich_condenser_(2).ppm" -o %OutFile% -method SCFauto -rct 0 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Friedrich_condenser_(2).ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FrnBtlEloKa_932_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct A3_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Fromsec4_1.ppm" -o %OutFile% -method SCFauto -rct A2_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Fromsec4_2.ppm" -o %OutFile% -method SCFauto -rct A3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FSsongmetal2-AAC-iTunes10.6.3-sweep.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -skip_rlc1 0 -log_name %LogFile% >> %res_en%
%EXE% -i "FSsongmetal2-AAC-iTunes10.6.3-sweep.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode none -pred none -segWidth 2500 -skip_rlc1 1 -postBWT IFC -IFCmax 5 -IFCreset 80 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FSsongmetal2-MP3-LAME3.99.5-sweep.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -skip_rlc1 0 -log_name %LogFile% >> %res_en%
%EXE% -i "FSsongmetal2-MP3-LAME3.99.5-sweep.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode none -pred none -segWidth 2500 -skip_rlc1 1 -postBWT IFC -IFCmax 3 -IFCreset 10 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FSsongmetal2-Vorbis-aoTuVb6.03-sweep.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -skip_rlc1 0 -log_name %LogFile% >> %res_en%
%EXE% -i "FSsongmetal2-Vorbis-aoTuVb6.03-sweep.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode none -pred none -segWidth 2500 -skip_rlc1 1 -postBWT IFC -IFCmax 5 -IFCreset 80 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "FullSkyAstronomySoftwareLogo.ppm" -o %OutFile% -method HXC2 -rct A1_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 4 -tolR3 4 -tolR4 15 -tolR5 15 -tolG0 1 -tolG1 1 -tolG2 12 -tolG3 12 -tolG4 17 -tolG5 17 -tolB0 6 -tolB1 6 -tolB2 36 -tolB3 36 -tolB4 221 -tolB5 221 -log_name %LogFile% >> %res_en%
%EXE% -i "fusion-pet-mri-ct-2.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "g_black_hole.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ga-high-10.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Galeodes_(Lydekker).ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Galeon_2.0.2.ppm" -o %OutFile% -method SCFauto -rct A5_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gallinula_chloropus(PSF).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 11 -tolR1 11 -tolR2 33 -tolR3 33 -tolR4 55 -tolR5 55 -log_name %LogFile% >> %res_en%
%EXE% -i "Gamma_0_5.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gamma_2_0.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred PAETH -HXC2auto 0 -HXC2increasefac 2 -tolR0 0 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 0 -tolR5 1 -tolG0 0 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 0 -tolG5 1 -tolB0 0 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 0 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gamma0.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "gate.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "GebMusKorps.ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "GebPiUstgKp_240_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gegenseitigkeit_.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gem-BMP_1000.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gendergap2011.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "GENERALGCRINCON.ppm" -o %OutFile% -method CoBaLP2 -rct A2_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Geneviève_Asse_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Geode10.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Geode3.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_IV._The_long-nosed_Stranger_of_Strasburg.ppm" -o %OutFile% -method LOCO -rct RGB -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_IV._The_long-nosed_Stranger_of_Strasburg.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_IV._The_long-nosed_Stranger_of_Strasburg.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_IV._The_long-nosed_Stranger_of_Strasburg.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_V._My_Uncle_Toby_on_his_Hobby-horse.ppm" -o %OutFile% -method LOCO -rct A2_1 -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_V._My_Uncle_Toby_on_his_Hobby-horse.ppm" -o %OutFile% -method LOCO -rct A8_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_V._My_Uncle_Toby_on_his_Hobby-horse.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_1 -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A6_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "George_Cruikshank_-_Tristram_Shandy,_Plate_VIII._The_Smoking_Batteries.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Geschäftsleitung..ppm" -o %OutFile% -method HXC2 -rct A2_11 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "gg.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gimp2-2.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 1088 -skip_rlc1 1 -postBWT IFC -IFCmax 30 -IFCreset 100 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "girl.ppm" -o %OutFile% -method CoBaLP2 -rct RGB -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "girl1.ppm" -o %OutFile% -method CoBaLP2 -rct B4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gldt92.ppm" -o %OutFile% -method HXC2aut -rct B4_1 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gldt92.ppm" -o %OutFile% -method HXC2aut -rct A3_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gldt92.ppm" -o %OutFile% -method HXC2aut -rct A2_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gldt92.ppm" -o %OutFile% -method HXC2aut -rct A2_10 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gldt92.ppm" -o %OutFile% -method HXC2aut -rct PEI09 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gldt92.ppm" -o %OutFile% -method HXC2aut -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gldt92.ppm" -o %OutFile% -method HXC2aut -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gnome_3.2.ppm" -o %OutFile% -method CoBaLP2 -rct A4_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Go_c.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 16 -tolR1 16 -tolR2 32 -tolR3 32 -tolR4 64 -tolR5 64 -tolG0 7 -tolG1 7 -tolG2 29 -tolG3 29 -tolG4 29 -tolG5 29 -tolB0 10 -tolB1 10 -tolB2 27 -tolB3 27 -tolB4 27 -tolB5 27 -log_name %LogFile% >> %res_en%
%EXE% -i "Gocursus-dia1-2.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "goldhill.v.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "goldhill.y.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Graf_1.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gráfico.1..ppm" -o %OutFile% -method CoBaLP2 -rct A1_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Grafico.2..ppm" -o %OutFile% -method CoBaLP2 -rct A1_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gray1120.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode perm -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Gray219.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 7 -tolR3 7 -tolR4 7 -tolR5 7 -tolG0 1 -tolG1 1 -tolG2 8 -tolG3 8 -tolG4 8 -tolG5 8 -tolB0 0 -tolB1 0 -tolB2 8 -tolB3 8 -tolB4 8 -tolB5 8 -log_name %LogFile% >> %res_en%
%EXE% -i "Gray38.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 7 -tolR3 7 -tolR4 7 -tolR5 7 -tolG0 0 -tolG1 0 -tolG2 8 -tolG3 8 -tolG4 8 -tolG5 8 -tolB0 0 -tolB1 0 -tolB2 8 -tolB3 8 -tolB4 8 -tolB5 8 -log_name %LogFile% >> %res_en%
%EXE% -i "Gray577.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Grey_720-360.ppm" -o %OutFile% -method BPC -rct 0 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "grey-leaves-1g7u_small.ppm" -o %OutFile% -method CoBaLP2 -rct A1_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Grfsalinidad.3.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "GulfSunset1.ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Gwibber_2.0.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Hainaut_Centre.ppm" -o %OutFile% -method HXC2 -rct A3_1 -HXC2auto 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Hainaut_Centre.ppm" -o %OutFile% -method HXC2aut -rct A4_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Hainaut_Centre.ppm" -o %OutFile% -method HXC2aut -rct A5_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Haiti_administrative_map-fr.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Haiti_administrative_map-fr.ppm" -o %OutFile% -method HXC2 -rct A3_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 5 -tolR3 5 -tolR4 222 -tolR5 222 -tolG0 0 -tolG1 0 -tolG2 5 -tolG3 5 -tolG4 222 -tolG5 222 -tolB0 0 -tolB1 0 -tolB2 5 -tolB3 5 -tolB4 222 -tolB5 222 -log_name %LogFile% >> %res_en%
%EXE% -i "Haiti_blank_map_with_topography.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Haiti_blank_map_with_topography.ppm" -o %OutFile% -method HXC2aut -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Haiti_blank_map_with_topography.ppm" -o %OutFile% -method HXC2aut -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Haiti_departements_map-fr.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Haiti_departements_map-fr.ppm" -o %OutFile% -method HXC2 -rct A3_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 5 -tolR3 5 -tolR4 222 -tolR5 222 -tolG0 0 -tolG1 0 -tolG2 5 -tolG3 5 -tolG4 222 -tolG5 222 -tolB0 0 -tolB1 0 -tolB2 5 -tolB3 5 -tolB4 222 -tolB5 222 -log_name %LogFile% >> %res_en%
%EXE% -i "Haiti_road_map-fr.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Haiti_road_map-fr.ppm" -o %OutFile% -method HXC2aut -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Hangeul.ppm" -o %OutFile% -method SCFauto -rct A1_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Haniielezit.ppm" -o %OutFile% -method HXC2 -rct A3_1 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 7 -tolR1 7 -tolR2 33 -tolR3 33 -tolR4 68 -tolR5 68 -tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 -tolB0 3 -tolB1 3 -tolB2 8 -tolB3 8 -tolB4 11 -tolB5 11 -log_name %LogFile% >> %res_en%
%EXE% -i "happybirds.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "HashiwokakeroBeispiel.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "HashiwokakeroLösung.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Haxo_(Paris_Metro).ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Helminth_egg_sizes_1.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode kopt -pred none -segWidth 843 -skip_rlc1 1 -postBWT IFC -IFCmax 33 -IFCreset 133 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Helminth_egg_sizes_2.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode kopt -pred none -segWidth 843 -skip_rlc1 1 -postBWT IFC -IFCmax 33 -IFCreset 133 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Helminth_egg_sizes_3.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode kopt -pred none -segWidth 837 -skip_rlc1 1 -postBWT IFC -IFCmax 3 -IFCreset 10 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Hene-1.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Hene-2.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Henri_Cueco_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Henry_IV_stayed_here.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -log_name %LogFile% >> %res_en%
%EXE% -i "Henry_IV_stayed_here.ppm" -o %OutFile% -method CoBaLP2 -rct B1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Henry_IV_stayed_here.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "HeY-.ppm" -o %OutFile% -method HXC2 -rct A7_1 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 64 -tolR3 64 -tolR4 125 -tolR5 125 -tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "HeY-.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "HeY-.ppm" -o %OutFile% -method SCFauto -rct A8_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "HFlgStff_8.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "high-frequency-mobile-x-ray-camera.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "HighlanderpaintingCormag100.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Hist-gamma4.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 19 -tolR3 19 -tolR4 47 -tolR5 47 -log_name %LogFile% >> %res_en%
%EXE% -i "HistoriaBGCP12.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "History_Hungary_1.ppm" -o %OutFile% -method HXC2 -rct A6_3 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 112 -tolR3 112 -tolR4 112 -tolR5 112 -tolG0 2 -tolG1 2 -tolG2 12 -tolG3 12 -tolG4 110 -tolG5 110 -tolB0 2 -tolB1 2 -tolB2 10 -tolB3 10 -tolB4 81 -tolB5 81 -log_name %LogFile% >> %res_en%
%EXE% -i "HMusKorps_4_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "HMusKorps_7.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Hnl.4.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "home3.ppm" -o %OutFile% -method CoBaLP2 -rct A7_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "home4.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Homo_lumper_(francais).ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Homo_splitter_(deutsch).ppm" -o %OutFile% -method HXC2 -rct A2_9 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 3 -tolR3 3 -tolR4 10 -tolR5 10 -tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 -tolB0 0 -tolB1 0 -tolB2 3 -tolB3 3 -tolB4 6 -tolB5 6 -log_name %LogFile% >> %res_en%
%EXE% -i "Homo_splitter_(english).ppm" -o %OutFile% -method HXC2 -rct A2_9 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 3 -tolR3 3 -tolR4 9 -tolR5 9 -tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 -tolB0 0 -tolB1 0 -tolB2 3 -tolB3 3 -tolB4 6 -tolB5 6 -log_name %LogFile% >> %res_en%
%EXE% -i "hotel.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Hotelb.ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "hp3.ppm" -o %OutFile% -method BPC -rct RGB -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "HPC-3.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Hundraser_2.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Hundraser_3.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Huntington_University_.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "hyades_mosaic.ppm" -o %OutFile% -method CoBaLP2 -Idx 1 -Idx_sorting 1 -histMode none -pred none -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "hyades_mosaic_color.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 0 -tolR0 2 -tolR1 2 -tolR2 3 -tolR3 3 -tolR4 3 -tolR5 3 -tolG0 2 -tolG1 2 -tolG2 3 -tolG3 3 -tolG4 3 -tolG5 3 -tolB0 2 -tolB1 2 -tolB2 3 -tolB3 3 -tolB4 3 -tolB5 3 -log_name %LogFile% >> %res_en%
%EXE% -i "hyades_n.ppm" -o %OutFile% -method HXC2aut -rct B1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Iain_Hume_(2).ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ib_Braase_(1997).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "IC463831.ppm" -o %OutFile% -method HXC2aut -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Iceberg_1.ppm" -o %OutFile% -method LOCO -rct A9_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Idh_mundo.ppm" -o %OutFile% -method SCFauto -rct B3_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "IE9_Mobile_Acid_3.ppm" -o %OutFile% -method SCFauto -rct A1_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "If^eN'N'FbR(&).ppm" -o %OutFile% -method LOCO -rct A3_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Igloo0_7.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Illinois_Locator_Map.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "I-love-science1024x768.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im1.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im10.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im12.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im14.ppm" -o %OutFile% -method CoBaLP2 -rct A6_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im15.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im19.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im2.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im20.ppm" -o %OutFile% -method CoBaLP2 -rct A1_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im3.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "im4.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im5.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im6.ppm" -o %OutFile% -method CoBaLP2 -rct A3_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im7.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "im8.ppm" -o %OutFile% -method CoBaLP2 -rct A3_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Imac17pouces.ppm" -o %OutFile% -method CoBaLP2 -rct A9_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "image.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Image_3.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ImageConverter.ppm" -o %OutFile% -method TSIP -rct A3_1 -histMode none -pred ADAPT -blockWidth 18 -blockHeight Med_Paeth_Lin_GAP_CoBaLP -segWidth 1000 -segHeight 18 -interleaving YY_uvuv -skip_rlc1 0 -postBWT IFC -IFCmax 300 -IFCreset 5000 -skip_prec 0 -RLC2_zeros 0 -log_name %LogFile% >> %res_en%
%EXE% -i "ImageConverter_1.ppm" -o %OutFile% -method TSIP -rct A3_4 -histMode none -pred ADAPT -blockWidth 18 -blockHeight Med_Paeth_Lin_GAP_CoBaLP -segWidth 1000 -segHeight 18 -interleaving YY_uvuv -skip_rlc1 0 -postBWT IFC -IFCmax 300 -IFCreset 5000 -skip_prec 0 -RLC2_zeros 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Image-Languages-Europe.ppm" -o %OutFile% -method SCFauto -rct A6_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "img_aboutUS.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "img41.ppm" -o %OutFile% -method HXC2aut -rct A2_10 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "img-icons-a-png-official-macosx-leopard-icon-pack-daddy77-17777.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ImperiumRevelation2.0.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "index_abc.ppm" -o %OutFile% -method HXC2aut -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "index_abc.ppm" -o %OutFile% -method HXC2 -rct A2_11 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Indonesia_Raya_-_Sin_Po_Wekelijksche_Editie_-_10_Nov_1928.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Indonesia_Raya_-_Sin_Po_Wekelijksche_Editie_-_10_Nov_1928.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Indonesia_Raya_-_Sin_Po_Wekelijksche_Editie_-_10_Nov_1928.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Indonesia_Raya_-_Sin_Po_Wekelijksche_Editie_-_10_Nov_1928.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Indonesia_Raya_-_Sin_Po_Wekelijksche_Editie_-_10_Nov_1928.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Indonesia_Raya_-_Sin_Po_Wekelijksche_Editie_-_10_Nov_1928.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Indonesia_Raya_-_Sin_Po_Wekelijksche_Editie_-_10_Nov_1928.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Induzierte_pluripotente_Stammzellen.ppm" -o %OutFile% -method SCFauto -rct A2_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "InfS.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Inheritance_mixins.ppm" -o %OutFile% -method SCFauto -rct A1_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Inkscape_0.48.1.ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "inline-graphic-1.ppm" -o %OutFile% -method SCF -rct A3_1 -histMode comp -pred none -HXC2auto 0 -SCF_prediction 1 -SCF_directional 1 -SCF_stage2tuning 1 -SCF_colourMAP 1 -HXC2increasefac 1 -tolR0 2 -tolR1 2 -tolR2 8 -tolR3 8 -tolR4 8 -tolR5 8 -tolG0 3 -tolG1 3 -tolG2 10 -tolG3 10 -tolG4 22 -tolG5 22 -tolB0 5 -tolB1 5 -tolB2 8 -tolB3 8 -tolB4 8 -tolB5 8 -log_name %LogFile% >> %res_en%
%EXE% -i "Inout1.ppm" -o %OutFile% -method SCFauto -rct A4_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Inout3.ppm" -o %OutFile% -method CoBaLP2 -rct A3_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "InstallNova2_1.ppm" -o %OutFile% -method CoBaLP2 -rct A2_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "institute_of_cognitive_science_osnabrueck_0_standard.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Institutionen_HRR.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Institutionen_HRR.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "InstRgt_1.ppm" -o %OutFile% -method CoBaLP2 -rct A8_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Interacting_Galaxies_2.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Interacting_Galaxies_3.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Invasionos1_0_2.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "InVivoTarget1.ppm" -o %OutFile% -method HXC2 -rct A1_1 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 6 -tolR1 6 -tolR2 10 -tolR3 10 -tolR4 10 -tolR5 10 -tolG0 10 -tolG1 10 -tolG2 10 -tolG3 10 -tolG4 10 -tolG5 10 -tolB0 10 -tolB1 10 -tolB2 10 -tolB3 10 -tolB4 10 -tolB5 10 -log_name %LogFile% >> %res_en%
%EXE% -i "IPNA.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Irene_Adler.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 13 -tolR1 13 -tolR2 13 -tolR3 13 -tolR4 13 -tolR5 13 -log_name %LogFile% >> %res_en%
%EXE% -i "ISN_10020_CSRT_2007_transcript_Pg_2.ppm" -o %OutFile% -method TSIP -rct 0 -histMode comp -pred none -segWidth 792 -skip_rlc1 0 -postBWT IFC -IFCmax 241 -IFCreset 3856 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ISN_10020_CSRT_2007_transcript_Pg_22.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "ISN_10020_CSRT_2007_transcript_Pg_4.ppm" -o %OutFile% -method TSIP -rct 0 -histMode comp -pred none -segWidth 792 -skip_rlc1 0 -postBWT IFC -IFCmax 28 -IFCreset 3856 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ISN_200_ARB_2006_transcript_Pg_26.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 62 -tolR3 62 -tolR4 62 -tolR5 62 -log_name %LogFile% >> %res_en%
%EXE% -i "isuoef.ppm" -o %OutFile% -method SCFauto -rct A3_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Itop1_0.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Jaceys-0.ppm" -o %OutFile% -method SCFauto -rct A1_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Jaceys-2.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Jacques_Busse_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Japanes_Fan_(Hakusen).ppm" -o %OutFile% -method CoBaLP2 -rct B4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "JCSY(3).ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "JCSYgarden(2).ppm" -o %OutFile% -method CoBaLP2 -rct A3_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Jean_Bertholle_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "JgBrig_37_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "JgBtl_101_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "jones_street.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "jones_street_2.ppm" -o %OutFile% -method CoBaLP2 -rct A6_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "josephs_coat_of_many_colours-dennis_brown-joe_gibbs-jgml-6008a.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Joyce2006_.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "JULIA3_.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 5 -tolR3 5 -tolR4 222 -tolR5 222 -log_name %LogFile% >> %res_en%
%EXE% -i "Jupiter_Earth_Comparison.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k01.ppm" -o %OutFile% -method LOCO -rct A7_10 -log_name %LogFile% >> %res_en%
%EXE% -i "k01.ppm" -o %OutFile% -method LOCO -rct A8_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k01.ppm" -o %OutFile% -method LOCO -rct A9_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k01.ppm" -o %OutFile% -method LOCO -rct A8_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k02.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -skip_rlc1 0 -log_name %LogFile% >> %res_en%
%EXE% -i "k02.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 1536 -skip_rlc1 1 -postBWT IFC -IFCmax 3 -IFCreset 10 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k03.ppm" -o %OutFile% -method CoBaLP2 -rct A6_10 -log_name %LogFile% >> %res_en%
%EXE% -i "k03.ppm" -o %OutFile% -method CoBaLP2 -rct A6_10 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k04.ppm" -o %OutFile% -method LOCO -rct A6_10 -log_name %LogFile% >> %res_en%
%EXE% -i "k04.ppm" -o %OutFile% -method LOCO -rct A6_3 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k04.ppm" -o %OutFile% -method LOCO -rct A2_3 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k04.ppm" -o %OutFile% -method LOCO -rct A6_3 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k04.ppm" -o %OutFile% -method LOCO -rct A2_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k05.ppm" -o %OutFile% -method LOCO -rct A2_10 -log_name %LogFile% >> %res_en%
%EXE% -i "k05.ppm" -o %OutFile% -method LOCO -rct A7_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k05.ppm" -o %OutFile% -method LOCO -rct A5_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k05_U.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "k05_U_9bit.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "k05_U_9bit.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k05_U.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k05_V.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "k05_V.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k05_Y.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "k05_Y.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k06.ppm" -o %OutFile% -method LOCO -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "k06.ppm" -o %OutFile% -method LOCO -rct A4_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k06.ppm" -o %OutFile% -method LOCO -rct A1_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k06.ppm" -o %OutFile% -method LOCO -rct A5_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k07.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -log_name %LogFile% >> %res_en%
%EXE% -i "k07.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k08.ppm" -o %OutFile% -method LOCO -rct A2_4 -log_name %LogFile% >> %res_en%
%EXE% -i "k08.ppm" -o %OutFile% -method LOCO -rct A7_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k08.ppm" -o %OutFile% -method LOCO -rct A8_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k08.ppm" -o %OutFile% -method LOCO -rct A4_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k08.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k09.ppm" -o %OutFile% -method LOCO -rct A2_4 -log_name %LogFile% >> %res_en%
%EXE% -i "k09.ppm" -o %OutFile% -method LOCO -rct PEI09 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k10.ppm" -o %OutFile% -method CoBaLP2 -rct A5_4 -log_name %LogFile% >> %res_en%
%EXE% -i "k10.ppm" -o %OutFile% -method CoBaLP2 -rct A5_4 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k11.ppm" -o %OutFile% -method LOCO -rct A5_4 -log_name %LogFile% >> %res_en%
%EXE% -i "k11.ppm" -o %OutFile% -method LOCO -rct A5_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k11.ppm" -o %OutFile% -method LOCO -rct A7_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k11.ppm" -o %OutFile% -method LOCO -rct A1_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k12.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -log_name %LogFile% >> %res_en%
%EXE% -i "k12.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k13.ppm" -o %OutFile% -method LOCO -rct PEI09 -log_name %LogFile% >> %res_en%
%EXE% -i "k13.ppm" -o %OutFile% -method LOCO -rct A6_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k13.ppm" -o %OutFile% -method LOCO -rct A2_7 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k13.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k14.ppm" -o %OutFile% -method LOCO -rct A2_4 -log_name %LogFile% >> %res_en%
%EXE% -i "k14.ppm" -o %OutFile% -method LOCO -rct A4_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k14.ppm" -o %OutFile% -method LOCO -rct A1_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k14.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k15.ppm" -o %OutFile% -method LOCO -rct A2_4 -log_name %LogFile% >> %res_en%
%EXE% -i "k15.ppm" -o %OutFile% -method LOCO -rct A1_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k15.ppm" -o %OutFile% -method LOCO -rct A1_12 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k15.ppm" -o %OutFile% -method LOCO -rct A5_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k16.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -log_name %LogFile% >> %res_en%
%EXE% -i "k16.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k17.ppm" -o %OutFile% -method CoBaLP2 -rct A7_7 -log_name %LogFile% >> %res_en%
%EXE% -i "k17.ppm" -o %OutFile% -method CoBaLP2 -rct A7_7 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k18.ppm" -o %OutFile% -method LOCO -rct A7_7 -log_name %LogFile% >> %res_en%
%EXE% -i "k18.ppm" -o %OutFile% -method LOCO -rct A6_7 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k18.ppm" -o %OutFile% -method LOCO -rct A3_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k18.ppm" -o %OutFile% -method LOCO -rct A6_7 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k19.ppm" -o %OutFile% -method LOCO -rct A6_7 -log_name %LogFile% >> %res_en%
%EXE% -i "k19.ppm" -o %OutFile% -method LOCO -rct A4_7 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k19.ppm" -o %OutFile% -method LOCO -rct A4_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k19.ppm" -o %OutFile% -method LOCO -rct A6_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k20.ppm" -o %OutFile% -method LOCO -rct A6_10 -log_name %LogFile% >> %res_en%
%EXE% -i "k20.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k20.ppm" -o %OutFile% -method LOCO -rct A2_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k20.ppm" -o %OutFile% -method LOCO -rct A8_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k20.ppm" -o %OutFile% -method LOCO -rct A6_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k21.ppm" -o %OutFile% -method CoBaLP2 -rct A5_7 -log_name %LogFile% >> %res_en%
%EXE% -i "k21.ppm" -o %OutFile% -method CoBaLP2 -rct A5_7 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "k22.ppm" -o %OutFile% -method LOCO -rct A5_7 -log_name %LogFile% >> %res_en%
%EXE% -i "k22.ppm" -o %OutFile% -method LOCO -rct A2_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k22.ppm" -o %OutFile% -method LOCO -rct A4_7 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k22.ppm" -o %OutFile% -method LOCO -rct A4_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k23.ppm" -o %OutFile% -method LOCO -rct A4_4 -log_name %LogFile% >> %res_en%
%EXE% -i "k23.ppm" -o %OutFile% -method LOCO -rct A4_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k23.ppm" -o %OutFile% -method LOCO -rct A2_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k23.ppm" -o %OutFile% -method LOCO -rct A4_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k23.ppm" -o %OutFile% -method LOCO -rct A6_5 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "k24.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -log_name %LogFile% >> %res_en%
%EXE% -i "k24.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kannur.ppm" -o %OutFile% -method SCFauto -rct A3_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kano1.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Kard_Dziwisz_COA.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_des_Großdeutschen_Reiches_1943.ppm" -o %OutFile% -method CoBaLP2 -rct A5_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_des_Großdeutschen_Reiches_1943.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_des_Großdeutschen_Reiches_1943.ppm" -o %OutFile% -method HXC2aut -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_Helvetik_4.ppm" -o %OutFile% -method HXC2aut -rct A2_11 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_Helvetik_4.ppm" -o %OutFile% -method HXC2aut -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_Helvetik_4.ppm" -o %OutFile% -method HXC2aut -rct PEI09 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_Stadtstaat_Bern_version-für-svgexport.ppm" -o %OutFile% -method HXC2aut -rct PEI09 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_Stadtstaat_Bern_version-für-svgexport.ppm" -o %OutFile% -method HXC2aut -rct A3_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_Stadtstaat_Bern_version-für-svgexport.ppm" -o %OutFile% -method HXC2aut -rct A9_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_Stadtstaat_Bern_version-für-svgexport.ppm" -o %OutFile% -method HXC2aut -rct A6_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_Stadtstaat_Bern_version-für-svgexport.ppm" -o %OutFile% -method HXC2aut -rct A7_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_Stadtstaat_Bern_version-für-svgexport.ppm" -o %OutFile% -method HXC2aut -rct A2_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_Stadtstaat_Bern_version-für-svgexport.ppm" -o %OutFile% -method HXC2aut -rct A2_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte_von_Senftenberg.ppm" -o %OutFile% -method SCFauto -rct A5_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte-DFB-Regionalverbände.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte-DFB-Regionalverbände.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 1653 -skip_rlc1 1 -postBWT IFC -IFCmax 5 -IFCreset 80 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Karte-DFB-Regionalverbände.ppm" -o %OutFile% -method HXC2 -rct A2_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 5 -tolR3 5 -tolR4 222 -tolR5 222 -tolG0 0 -tolG1 0 -tolG2 5 -tolG3 5 -tolG4 222 -tolG5 222 -tolB0 0 -tolB1 0 -tolB2 5 -tolB3 5 -tolB4 222 -tolB5 222 -log_name %LogFile% >> %res_en%
%EXE% -i "kate_nash_630x1200_b01_U.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kate_nash_630x1200_b01_U_rot.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kate_nash_630x1200_b01_V.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kate_nash_630x1200_b01_V_rotated.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kate_nash_630x1200_b01_Y.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kathleen_Wynne.ppm" -o %OutFile% -method CoBaLP2 -rct A1_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kbm-oslocentralxmas-34t.ppm" -o %OutFile% -method HXC2 -rct A6_11 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 9 -tolR1 7 -tolR2 9 -tolR3 11 -tolR4 15 -tolR5 16 -tolG0 7 -tolG1 4 -tolG2 7 -tolG3 8 -tolG4 10 -tolG5 8 -tolB0 3 -tolB1 1 -tolB2 4 -tolB3 2 -tolB4 2 -tolB5 2 -log_name %LogFile% >> %res_en%
%EXE% -i "KDE_3.5.ppm" -o %OutFile% -method SCFauto -rct A2_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "KDE_4.ppm" -o %OutFile% -method BPC -rct B1_2 -histMode none -pred PAETH -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Keb'_Mo'.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Keltisch_sprachen.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kerguelen_topographic_blank_map.ppm" -o %OutFile% -method HXC2aut -rct RGB -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kerguelen_topographic_blank_map.ppm" -o %OutFile% -method HXC2aut -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kerguelen_topographic_blank_map.ppm" -o %OutFile% -method HXC2aut -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "keyboard.ppm" -o %OutFile% -method HXC2 -rct A2_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Khavinson_V.K.2.ppm" -o %OutFile% -method CoBaLP2 -rct A2_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kievan-rus-1015-1113-(de).ppm" -o %OutFile% -method HXC2aut -rct A2_11 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kievan-rus-1015-1113-(de).ppm" -o %OutFile% -method HXC2aut -rct A7_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kievan-rus-1015-1113-(de).ppm" -o %OutFile% -method HXC2aut -rct A3_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kievan-rus-1015-1113-(de).ppm" -o %OutFile% -method HXC2aut -rct A4_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kievan-rus-1015-1113-(de).ppm" -o %OutFile% -method HXC2aut -rct A2_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kino1.3.4.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "KlausWeber-USPatent3518014-Fig1-3.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 8 -tolR3 8 -tolR4 27 -tolR5 27 -log_name %LogFile% >> %res_en%
%EXE% -i "Knife_(3263343357).ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Knife_(3264170936).ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Knife_(3264175544).ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim01.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim02.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim03.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim04.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim04_Cb.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim04_Cb.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim04_Cr.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim04_Cr.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim05.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim06.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim07.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim08.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim09.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim10.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim11.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim12.ppm" -o %OutFile% -method CoBaLP2 -rct A1_10 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim13.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim14.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim15.ppm" -o %OutFile% -method CoBaLP2 -rct A1_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim16.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim17.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim18.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim19.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim20.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim21.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim22.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim23.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "kodim24.ppm" -o %OutFile% -method CoBaLP2 -rct A2_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Korsvegen_(3).ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "krb-insertpostersanta-07t.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kriechingen.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "KSpaceDuel1.1.ppm" -o %OutFile% -method HXC2 -rct A3_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "KSpaceDuel1.1.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "KSpaceDuel1.1.ppm" -o %OutFile% -method SCFauto -rct A6_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "KSpaceDuel1.1.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kunstindustrimuseet_3.ppm" -o %OutFile% -method CoBaLP2 -Idx 1 -Idx_sorting 1 -histMode kopt -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kunstindustrimuseet_4.ppm" -o %OutFile% -method CoBaLP2 -rct A8_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Kurd_Man.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Kurd_Woman.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "LaDiscussionScientifiqueDuCorset81_.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "LaDiscussionScientifiqueDuCorset93_.ppm" -o %OutFile% -method LOCO -rct 0 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Lady+Gaga+PNG.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ladybug_landscape.ppm" -o %OutFile% -method HXC2aut -rct B6_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "lake1.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "lake1.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "lake1.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "lake1.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Landscape.ppm" -o %OutFile% -method HXC2 -rct A9_12 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 6 -tolR0 4 -tolR1 4 -tolR2 4 -tolR3 4 -tolR4 4 -tolR5 4 -tolG0 4 -tolG1 4 -tolG2 4 -tolG3 4 -tolG4 4 -tolG5 4 -tolB0 4 -tolB1 4 -tolB2 4 -tolB3 4 -tolB4 4 -tolB5 4 -log_name %LogFile% >> %res_en%
%EXE% -i "Landscape_002.ppm" -o %OutFile% -method BPC -rct A3_1 -histMode none -pred ADAPT -blockWidth 3951 -blockHeight 0 -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Landscape_Full_HD_by_depARTed89.ppm" -o %OutFile% -method CoBaLP2 -rct A3_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "langfristchart_historischer_chart_orangensaft.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Langs_N.Amer.ppm" -o %OutFile% -method SCFauto -rct A6_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "laser_scanning_tomography.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 1 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "lawn-v6h3.ppm" -o %OutFile% -method CoBaLP2 -rct A6_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "leaves-12.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "leaves-6n3y.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "leaves-kijj7.ppm" -o %OutFile% -method CoBaLP2 -rct A9_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "leaves-mval_small.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Legendscarscup_1.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "LegvEmmaus_1.ppm" -o %OutFile% -method SCFauto -rct A1_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Leopoldo_Nóvoa_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A2_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Letíme.ppm" -o %OutFile% -method CoBaLP2 -rct A3_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Lib5$.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "LietuvosSeniunijos.ppm" -o %OutFile% -method CoBaLP2 -rct A8_2 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "LietuvosSeniunijos.ppm" -o %OutFile% -method HXC2aut -rct A3_10 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "life_science.ppm" -o %OutFile% -method HXC2aut -rct B2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "light_world-1.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode none -pred none -segWidth 100000 -skip_rlc1 1 -postBWT IFC -IFCmax 3 -IFCreset 7 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "light_world-1.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode none -pred none -segWidth 2048 -skip_rlc1 1 -postBWT IFC -IFCmax 3 -IFCreset 7 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Limitesdeplacastectónicas.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "LinkAPix_1.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "LinkAPix_3.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Lins..ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "LinuxLibertine-2.1.0.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Localizador-).ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "logo.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Logo_3.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Logo_precise_name.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Logo_Socialize-.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 25 -tolG3 25 -tolG4 38 -tolG5 38 -tolB0 0 -tolB1 0 -tolB2 25 -tolB3 25 -tolB4 38 -tolB5 38 -log_name %LogFile% >> %res_en%
%EXE% -i "logo-pflegetag-gross-rgb-bmp.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode none -pred none -segWidth 1080 -skip_rlc1 1 -postBWT MTF -MTF_val 0 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Lonesomehurst1911.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Louis_Pons_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A6_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ls-html-screen-picker-003.ppm" -o %OutFile% -method HXC2aut -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "lunar-lander.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Luxembourg_(he).ppm" -o %OutFile% -method SCFauto -rct A5_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "-M0.#JE9M9-.ppm" -o %OutFile% -method LOCO -rct A2_11 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "M0025_kappaxray_error.ppm" -o %OutFile% -method CoBaLP2 -Idx 1 -Idx_sorting 0 -histMode kopt -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "M175_haplogroup_map,_Asia.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "M2100102-Colour_SPECT_scan_of_brain_during_migraine_attack-SPL.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "M2400146-False-colour_CT_scan_of_a_head_showing_nasal_polyp-SPL.ppm" -o %OutFile% -method CoBaLP2 -rct A8_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "macro-765.ppm" -o %OutFile% -method CoBaLP2 -rct A8_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "mad-scientist-clipart.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Magy_(Cartoon_character).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Malikstanislaw1.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Mandelbrot_Leminiscates_1.ppm" -o %OutFile% -method HXC2 -rct 0 -HXC2auto 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Mandelbrot_Leminiscates_1.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 110 -tolR3 110 -tolR4 220 -tolR5 220 -log_name %LogFile% >> %res_en%
%EXE% -i "Mandelbrot_Leminiscates_1.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "map_00.ppm" -o %OutFile% -method SCFauto -rct A1_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_01.ppm" -o %OutFile% -method HXC2 -rct A8_11 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 110 -tolR3 110 -tolR4 220 -tolR5 220 -tolG0 0 -tolG1 0 -tolG2 110 -tolG3 110 -tolG4 220 -tolG5 220 -tolB0 0 -tolB1 0 -tolB2 110 -tolB3 110 -tolB4 220 -tolB5 220 -log_name %LogFile% >> %res_en%
%EXE% -i "map_02.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_03.ppm" -o %OutFile% -method SCFauto -rct A2_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_04.ppm" -o %OutFile% -method SCFauto -rct A1_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_05.ppm" -o %OutFile% -method SCFauto -rct A1_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_06.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_07.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_08.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_09.ppm" -o %OutFile% -method SCFauto -rct A3_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_10.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Map_118230550922074.ppm" -o %OutFile% -method HXC2 -rct A3_11 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 2 -tolR1 2 -tolR2 2 -tolR3 2 -tolR4 44 -tolR5 44 -tolG0 2 -tolG1 2 -tolG2 2 -tolG3 2 -tolG4 44 -tolG5 44 -tolB0 2 -tolB1 2 -tolB2 2 -tolB3 2 -tolB4 44 -tolB5 44 -log_name %LogFile% >> %res_en%
%EXE% -i "map_Florenz_1.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_Florenz_2.ppm" -o %OutFile% -method SCFauto -rct A7_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Map_of_Allegheny_County,_Pennsylvania.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Map_of_Allegheny_County,_Pennsylvania.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Map_of_Allegheny_County,_Pennsylvania.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MAP_Strandbahn_Rostock-WAR-MGH_1910-1945.ppm" -o %OutFile% -method HXC2aut -rct 0 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MAP_Strandbahn_Rostock-WAR-MGH_1910-1945.ppm" -o %OutFile% -method HXC2aut -rct A7_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MAP_Strandbahn_Rostock-WAR-MGH_1910-1945.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 2 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MAP_Strandbahn_Rostock-WAR-MGH_1910-1945.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 1 -tolG1 2 -tolG2 1 -tolG3 2 -tolG4 1 -tolG5 2 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MAP_Strandbahn_Rostock-WAR-MGH_1910-1945.ppm" -o %OutFile% -method HXC2aut -rct A4_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "map_vicinity.ppm" -o %OutFile% -method HXC2aut -rct A5_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Mapa-1-.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MapLocalitationMoguer.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MarkovGraph-1.ppm" -o %OutFile% -method SCFauto -rct A3_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Maronesa_cow_1.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Marsalkansauva_(1928).ppm" -o %OutFile% -method TSIP -rct A8_4 -histMode comp -pred MEDPAETH -segWidth 523 -interleaving YY_uvuv -skip_rlc1 0 -postBWT IFC -IFCmax 192 -IFCreset 3072 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Martha_Boto_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MaterialpicD..ppm" -o %OutFile% -method HXC -rct A5_1 -histMode comp -pred none -HXCauto 0 -tol 0 -tolOff1 1 -tolOff2 1 -HXCincreasefac 2 -log_name %LogFile% >> %res_en%
%EXE% -i "Maya_.ppm" -o %OutFile% -method CoBaLP2 -rct A5_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Mayas.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Mazowieckie-administracja.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Mazowieckie-administracja.ppm" -o %OutFile% -method HXC2aut -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Mazowieckie-administracja.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MCL_1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "mcn-cokedublin-43s.ppm" -o %OutFile% -method HXC2aut -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "mcn-cokedublin-43s.ppm" -o %OutFile% -method HXC2 -rct A2_6 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 1 -tolR3 1 -tolR4 5 -tolR5 5 -tolG0 0 -tolG1 0 -tolG2 1 -tolG3 1 -tolG4 6 -tolG5 6 -tolB0 0 -tolB1 0 -tolB2 1 -tolB3 1 -tolB4 4 -tolB5 4 -log_name %LogFile% >> %res_en%
%EXE% -i "mcn-cokedublin-43s.ppm" -o %OutFile% -method HXC2 -rct A2_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MD82v1.0.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MD82v1.0.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Me-.ppm" -o %OutFile% -method SCFauto -rct A3_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Meandros.ppm" -o %OutFile% -method HXC2 -rct A1_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "medical2.ppm" -o %OutFile% -method CoBaLP2 -rct A8_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "mediothek_landscape.ppm" -o %OutFile% -method CoBaLP2 -rct A2_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "meditate_fel.ppm" -o %OutFile% -method HXC2aut -rct A2_11 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "meditate_fel.ppm" -o %OutFile% -method HXC2aut -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "meditate_fel.ppm" -o %OutFile% -method HXC2aut -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "meditate_fel.ppm" -o %OutFile% -method HXC2aut -rct A9_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "meditate_fel.ppm" -o %OutFile% -method HXC2aut -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Merian-PNG.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Mesoamérica.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Metria.ppm" -o %OutFile% -method CoBaLP2 -rct A5_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MetroTorino.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MICFig-121.ppm" -o %OutFile% -method SCFauto -rct A1_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MICFig-122.ppm" -o %OutFile% -method SCFauto -rct A3_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Michelangelo_David.ppm" -o %OutFile% -method CoBaLP2 -rct B4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Micromega-science-news-653372_1280_960.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Microscopic_Images_3.ppm" -o %OutFile% -method CoBaLP2 -rct A9_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MikeOS-2.0.0.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode perm -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Milky_Way_Spiral_Arms.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Miri_Yampolsky_(pianist).ppm" -o %OutFile% -method CoBaLP2 -rct A2_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "misc-09.ppm" -o %OutFile% -method CoBaLP2 -rct A2_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MITP_USA_2008DEC_tomography_IDV_western_US_2_isosurfaces_planView_300km.ppm" -o %OutFile% -method SCFauto -rct B1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMarmyflag.ppm" -o %OutFile% -method HXC2 -rct A2_1 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 14 -tolR1 14 -tolR2 44 -tolR3 44 -tolR4 44 -tolR5 44 -tolG0 6 -tolG1 6 -tolG2 72 -tolG3 72 -tolG4 72 -tolG5 72 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A4_10 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A8_5 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A4_1 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A2_4 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A4_4 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A4_4 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A4_4 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A2_7 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode none -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A4_4 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A4_10 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A4_4 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A2_7 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A2_4 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode none -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MMSposter-large.ppm" -o %OutFile% -method BPC -rct A2_4 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "MOLT_7.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 42 -tolR3 42 -tolR4 48 -tolR5 48 -log_name %LogFile% >> %res_en%
%EXE% -i "MonogramaBGCP.ppm" -o %OutFile% -method CoBaLP2 -rct B2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Moonalphabet.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "MozillaFirefox3.6.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ms.XIX.17-3_Cover_4.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -log_name %LogFile% >> %res_en%
%EXE% -i "Ms.XIX.17-3_Cover_4.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Muehlstr_5_Burgalben_2.ppm" -o %OutFile% -method LOCO -rct A4_10 -log_name %LogFile% >> %res_en%
%EXE% -i "Muehlstr_5_Burgalben_2.ppm" -o %OutFile% -method LOCO -rct A1_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Muehlstr_5_Burgalben_2.ppm" -o %OutFile% -method LOCO -rct A1_7 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Muehlstr_5_Burgalben_2.ppm" -o %OutFile% -method LOCO -rct A9_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Muehlstr_5_Burgalben_2.ppm" -o %OutFile% -method LOCO -rct A7_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Muehlstr_5_Burgalben_2.ppm" -o %OutFile% -method LOCO -rct A9_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "music.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "NachrSBw..ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Name.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "namenlos.ppm" -o %OutFile% -method BPC -rct A2_10 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "NaPaa.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "nasch_dom_roccija.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Nasseraldinshah3.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Nautilus_3.2.1.ppm" -o %OutFile% -method SCFauto -rct A9_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ncat_fig7_8.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 850 -skip_rlc1 1 -postBWT MTF -MTF_val 0 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "NDM-1.ppm" -o %OutFile% -method CoBaLP2 -Idx 1 -Idx_sorting 1 -log_name %LogFile% >> %res_en%
%EXE% -i "NDM-1.ppm" -o %OutFile% -method CoBaLP2 -rct A5_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "NDM-1.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "NDsign_2.ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "netscape.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Netzentwuerfe.ppm" -o %OutFile% -method SCFauto -rct A2_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "New-Science.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Niele_Toroni_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "night-379.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Nikotin-Rezeptoren_Flowchart.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Nodoarbol-bc++.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "noir-diabolik-guitar-hommess_design.ppm" -o %OutFile% -method SCFauto -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Northeast_Caucasian_languages.ppm" -o %OutFile% -method SCFauto -rct A2_10 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Nova.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "nqueens.ppm" -o %OutFile% -method HXC -Idx 1 -Idx_sorting 0 -histMode none -pred none -HXCauto 0 -tol 0 -tolOff1 0 -tolOff2 1 -HXCincreasefac 2 -log_name %LogFile% >> %res_en%
%EXE% -i "NuclideMap.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "NuclideMap.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "NuclideMap.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "NuclideMap.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "NuclideMap.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "NuclideMap.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "NuclideMap.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "NuclideMap.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "NuclideMap.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Nukleationskern_Fluessigkeit.ppm" -o %OutFile% -method CoBaLP2 -rct B1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Nvlado_(cartoon).ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Oberschläsing_01.ppm" -o %OutFile% -method CoBaLP2 -rct A3_2 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Oblivion2007101513345379.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Oblivion2007101714023034.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Oblivion2007101714024170.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Oblivion2007101714040018.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "observatory.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ode_Bertrand_(2009).ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Olivier_Mosset_(2003).ppm" -o %OutFile% -method CoBaLP2 -rct A4_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Onu_.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "OpenCity0.0.5.ppm" -o %OutFile% -method CoBaLP2 -rct A6_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Opensuse11_4.ppm" -o %OutFile% -method CoBaLP2 -rct A7_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "OpenWrtWR0_9.ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "OpenWrtWR0_9.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "OpenWrtWR0_9.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "o-png24.ppm" -o %OutFile% -method CoBaLP2 -rct A2_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Oranienburg_Statuen_Kopie.ppm" -o %OutFile% -method SCFauto -rct A4_8 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "orm-makeposter2008-14t.ppm" -o %OutFile% -method HXC2aut -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Oscar_Gauthier_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A9_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Oscilloscope_.ppm" -o %OutFile% -method CoBaLP2 -rct A1_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ost-Deutschland_(1995).ppm" -o %OutFile% -method HXC2 -rct A3_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Oste014b..ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "OTSUTS_2.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ouanes_Amor_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A2_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "P.J..ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "P_C_B.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "P_thin_ext_cor_BlackBodyCT.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 6 -tolR3 6 -tolR4 26 -tolR5 26 -tolG0 0 -tolG1 0 -tolG2 7 -tolG3 7 -tolG4 66 -tolG5 66 -tolB0 0 -tolB1 0 -tolB2 25 -tolB3 25 -tolB4 83 -tolB5 83 -log_name %LogFile% >> %res_en%
%EXE% -i "p08.ppm" -o %OutFile% -method CoBaLP2 -rct RGB -log_name %LogFile% >> %res_en%
%EXE% -i "p08.ppm" -o %OutFile% -method CoBaLP2 -rct A8_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "p08.ppm" -o %OutFile% -method CoBaLP2 -rct A6_2 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "p08.ppm" -o %OutFile% -method CoBaLP2 -rct A8_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Padania.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Panic10.6.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 28 -tolR3 28 -tolR4 38 -tolR5 38 -log_name %LogFile% >> %res_en%
%EXE% -i "PanoramaOsilodaBunnari_.ppm" -o %OutFile% -method LOCO -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "PanoramaOsilodaBunnari_.ppm" -o %OutFile% -method LOCO -rct A3_10 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "party_4s.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Parvine_Curie_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Paso_1.ppm" -o %OutFile% -method SCFauto -rct A5_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Paso_2.ppm" -o %OutFile% -method SCFauto -rct A5_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Paso_3.ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pasodoble1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "pasodoble3.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pat_Andrea_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PATNIXONandQEII-png.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 587 -skip_rlc1 1 -postBWT IFC -IFCmax 55 -IFCreset 220 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "patter_cos.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "patter_cos_01.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode none -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pattern.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pattern_circles.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode none -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pattern_frac.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pattern_frac2.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pattern_lines.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "pattern_noise.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pattern_noise2.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pc.ppm" -o %OutFile% -method SCFauto -rct A1_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pc-z5e2.ppm" -o %OutFile% -method CoBaLP2 -rct A7_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pdn3.5.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pebbles.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pebbles.ppm" -o %OutFile% -method CoBaLP2 -rct A6_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pebbles.ppm" -o %OutFile% -method CoBaLP2 -rct A2_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "people-126.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "periodensystem-1280x887.ppm" -o %OutFile% -method HXC2 -rct A3_8 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Periodic_table_(German).ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Personalfrågor..ppm" -o %OutFile% -method CoBaLP2 -rct A1_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PetHoerr.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Pfaelzerwaldkarte.ppm" -o %OutFile% -method LOCO -rct A3_11 -log_name %LogFile% >> %res_en%
%EXE% -i "Pfaelzerwaldkarte.ppm" -o %OutFile% -method LOCO -rct A5_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Pfaelzerwaldkarte.ppm" -o %OutFile% -method LOCO -rct A8_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Pfaelzerwaldkarte.ppm" -o %OutFile% -method LOCO -rct A6_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Pfaelzerwaldkarte.ppm" -o %OutFile% -method LOCO -rct A7_7 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Pfaelzerwaldkarte.ppm" -o %OutFile% -method LOCO -rct A6_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Pfaelzerwaldkarte.ppm" -o %OutFile% -method LOCO -rct A7_7 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Pfeffer_Osmotische_Untersuchungen-1-2.ppm" -o %OutFile% -method LOCO -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Pfeffer_Osmotische_Untersuchungen-1-2.ppm" -o %OutFile% -method LOCO -rct 0 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Phillips_screw_head_1.ppm" -o %OutFile% -method SCFauto -rct A5_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Phillips_screwdriver_2.ppm" -o %OutFile% -method CoBaLP2 -rct B1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Photorespiration_cycle_(cs).ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "physics.ppm" -o %OutFile% -method CoBaLP2 -rct B1_2 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pie_charts_3d.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pierre_Fichet_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pig...ppm" -o %OutFile% -method BPC -rct 0 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Pineapple...ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pintara_tabrica_(male).ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pisg_.ppm" -o %OutFile% -method HXC2aut -rct A3_11 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pisg_.ppm" -o %OutFile% -method HXC2aut -rct A5_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pisg_.ppm" -o %OutFile% -method HXC2aut -rct B1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pixelart3jpg.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PlanoCotoprix..ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Plan-preahvihear-1.ppm" -o %OutFile% -method HXC2aut -rct A2_1 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Plan-preahvihear-1.ppm" -o %OutFile% -method HXC2aut -rct A1_10 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Plan-preahvihear-1.ppm" -o %OutFile% -method HXC2aut -rct A2_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "plant-fol2.ppm" -o %OutFile% -method CoBaLP2 -rct A8_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PLOSMapOfScience.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "png.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "png-8passes.ppm" -o %OutFile% -method HXC2 -rct A3_6 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 5 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "png-9passes.ppm" -o %OutFile% -method HXC2 -rct A3_6 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 220 -tolR5 220 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 220 -tolG5 220 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 220 -tolB5 220 -log_name %LogFile% >> %res_en%
%EXE% -i "PNG-climate.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "png-IceAlpha-ie5mac.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 1019 -skip_rlc1 1 -postBWT MTF -MTF_val 0 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PNGlogoColorLarge_.ppm" -o %OutFile% -method HXC2 -rct A2_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 6 -tolR3 6 -tolR4 11 -tolR5 11 -tolG0 1 -tolG1 1 -tolG2 39 -tolG3 39 -tolG4 59 -tolG5 59 -tolB0 64 -tolB1 64 -tolB2 64 -tolB3 64 -tolB4 128 -tolB5 128 -log_name %LogFile% >> %res_en%
%EXE% -i "PNGlogoColorLarge_.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PNGlogoColorLarge_.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PokerTH.ppm" -o %OutFile% -method SCFauto -rct A3_8 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Polen_in_den_Grenzen_vor_1660.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "police.ppm" -o %OutFile% -method SCFauto -rct A2_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pont_.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pontic_steppe_region_around_650_AD.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Porenkowo_Pomorsko_1.ppm" -o %OutFile% -method SCFauto -rct A3_8 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Portada.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Portal_2_Potatoe_Science_Kit.ppm" -o %OutFile% -method HXC2 -rct A1_11 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 0 -tolR0 0 -tolR1 0 -tolR2 20 -tolR3 20 -tolR4 110 -tolR5 110 -tolG0 0 -tolG1 0 -tolG2 50 -tolG3 50 -tolG4 110 -tolG5 110 -tolB0 0 -tolB1 0 -tolB2 30 -tolB3 30 -tolB4 110 -tolB5 110 -log_name %LogFile% >> %res_en%
%EXE% -i "PotentialSuperpowers.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Poty2007_voting_2.ppm" -o %OutFile% -method TSIP -rct PEI09 -histMode comp -pred ADAPT -blockWidth 20 -blockHeight Med_Paeth_Lin_GAP_CoBaLP -segWidth 1243 -segHeight 20 -interleaving YY_uvuv -skip_rlc1 0 -postBWT IFC -IFCmax 299 -IFCreset 4784 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "powerplant.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "powerplant.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Poymsoil_1.ppm" -o %OutFile% -method CoBaLP2 -rct A3_5 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PP_Medical_Dispenser.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PP_Skull.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "p-radiologist-art1_1467422c.ppm" -o %OutFile% -method CoBaLP2 -rct A6_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Previsat3.0.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Previsat3_1.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Prijepolje_1.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Primera2012-.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Primera2013-.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "prismIcon.ppm" -o %OutFile% -method CoBaLP2 -rct A8_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Projekt.ppm" -o %OutFile% -method SCFauto -rct A2_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Pronto-7.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PSM_V01_D008_Samuel_Finley_Breese_Morse.ppm" -o %OutFile% -method HXC2aut -rct 0 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PSM_V01_D008_Samuel_Finley_Breese_Morse.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PT_babeltower_01t1.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PTHNWIDE_WaveletsGamma_LVGamp16Contp8.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode none -pred none -segWidth 1000 -skip_rlc1 1 -postBWT IFC -IFCmax 3 -IFCreset 10 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Puffball.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "puzzle-p7z.ppm" -o %OutFile% -method CoBaLP2 -rct A8_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "pzbr11_0.16.10.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PzBrig_20_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct A8_3 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PzBtl_333_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PzBtl_343_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PzGrenBrig_1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "PZSL1851PlatePisces04.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Qfilterimage1.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "QLearning+.ppm" -o %OutFile% -method SCFauto -rct A1_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Quest3_.ppm" -o %OutFile% -method SCFauto -rct A3_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "QuickEK_5.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "QuickEK_5.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "QuickEK_5.ppm" -o %OutFile% -method SCFauto -rct A2_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Quintenzirkeldeluxe.ppm" -o %OutFile% -method HXC2 -rct A3_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 3 -tolR3 3 -tolR4 12 -tolR5 12 -tolG0 0 -tolG1 0 -tolG2 20 -tolG3 20 -tolG4 25 -tolG5 25 -tolB0 0 -tolB1 0 -tolB2 19 -tolB3 19 -tolB4 24 -tolB5 24 -log_name %LogFile% >> %res_en%
%EXE% -i "R.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rac01AUS(vertical).ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rackstagard_1.ppm" -o %OutFile% -method CoBaLP2 -rct A5_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rackstagard_2.ppm" -o %OutFile% -method CoBaLP2 -rct A5_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rackstagard_3.ppm" -o %OutFile% -method CoBaLP2 -rct A5_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Radhanites.ppm" -o %OutFile% -method CoBaLP2 -rct A9_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Radiodrome-simple-y-bw.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Radiodrome-sketch-lettered-colour.ppm" -o %OutFile% -method SCFauto -rct A3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RadioLublin.ppm" -o %OutFile% -method CoBaLP2 -rct A2_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Railroad1860.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RainbowRoad-double-150.ppm" -o %OutFile% -method CoBaLP2 -rct A8_9 -log_name %LogFile% >> %res_en%
%EXE% -i "RainbowRoad-double-150.ppm" -o %OutFile% -method CoBaLP2 -rct A8_9 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rangoli....ppm" -o %OutFile% -method HXC2 -rct 0 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 7 -tolR1 7 -tolR2 23 -tolR3 23 -tolR4 23 -tolR5 23 -log_name %LogFile% >> %res_en%
%EXE% -i "Ranking.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "rapeseed-z83.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ravenki2.3.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RAY-COLOR-3D_1328655128.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -log_name %LogFile% >> %res_en%
%EXE% -i "RAY-COLOR-3D_1328655128.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "-Re2Cl8-2-.ppm" -o %OutFile% -method HXC2 -rct A1_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 38 -tolR3 38 -tolR4 59 -tolR5 59 -tolG0 1 -tolG1 1 -tolG2 5 -tolG3 5 -tolG4 5 -tolG5 5 -tolB0 1 -tolB1 1 -tolB2 6 -tolB3 6 -tolB4 7 -tolB5 7 -log_name %LogFile% >> %res_en%
%EXE% -i "reactos_0.3.1_booklet_cover.ppm" -o %OutFile% -method BPC -rct A1_1 -log_name %LogFile% >> %res_en%
%EXE% -i "reactos_0.3.1_booklet_cover.ppm" -o %OutFile% -method CoBaLP2 -rct A3_6 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "reactos_0.3.1_booklet_cover.ppm" -o %OutFile% -method BPC -rct A3_8 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "reba.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Receiver12.2.ppm" -o %OutFile% -method BPC -rct RGB -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Receiver12_1.ppm" -o %OutFile% -method BPC -rct RGB -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Receiver12_3.ppm" -o %OutFile% -method SCFauto -rct A5_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rectangle_.ppm" -o %OutFile% -method SCFauto -rct PEI09 -histMode none -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "redhead.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Redstar3.0.ppm" -o %OutFile% -method BPC -rct A4_12 -histMode none -pred PAETH -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Regla21_2.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Regla21_2.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Regla21_2.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Regla21_2.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Regla21_2.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Regla21_2.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Regla21_2.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "RERR.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rhabdovirus.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Riskprem11-1.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Riskprem11-4.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RNAi_actin_12bit.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RNAi_dna_12bit.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RNAi_ph3_12bit.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RNaseH-cant_eat.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Romb+.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "roof-uh8k.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "rope-239.ppm" -o %OutFile% -method CoBaLP2 -rct A8_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rostock_Verwaltungsgliederung.ppm" -o %OutFile% -method HXC2aut -rct A8_4 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rostock_Verwaltungsgliederung.ppm" -o %OutFile% -method HXC2aut -rct A1_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rostock_Verwaltungsgliederung.ppm" -o %OutFile% -method HXC2aut -rct A3_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rostock_Verwaltungsgliederung.ppm" -o %OutFile% -method HXC2aut -rct B1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rostock_Verwaltungsgliederung.ppm" -o %OutFile% -method HXC2aut -rct A1_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RR5111-0211.ppm" -o %OutFile% -method SCFauto -rct A1_5 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RR5111-0212.ppm" -o %OutFile% -method CoBaLP2 -rct A5_7 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RR5111-0220.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RR5115-0055.ppm" -o %OutFile% -method CoBaLP2 -Idx 1 -Idx_sorting 0 -histMode none -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RR5115-0061.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode none -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RR5216-0060R.ppm" -o %OutFile% -method CoBaLP2 -rct A3_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RR5514-0066.ppm" -o %OutFile% -method CoBaLP2 -rct A7_5 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "rubbish-water-2ju8.ppm" -o %OutFile% -method CoBaLP2 -rct A6_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Rudder(Flap).ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ruecknahmegrafik.ppm" -o %OutFile% -method HXC2 -rct A2_6 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 110 -tolR3 110 -tolR4 330 -tolR5 330 -tolG0 0 -tolG1 0 -tolG2 110 -tolG3 110 -tolG4 330 -tolG5 330 -tolB0 0 -tolB1 0 -tolB2 110 -tolB3 110 -tolB4 330 -tolB5 330 -log_name %LogFile% >> %res_en%
%EXE% -i "ruler.ppm" -o %OutFile% -method BPC -rct 0 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "rumpelklas.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RussiaFarEastNumbered.ppm" -o %OutFile% -method HXC2aut -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Russian_checkers_2.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Russianpeople3_1.ppm" -o %OutFile% -method CoBaLP2 -rct A9_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RX_J0852-4622HESS.ppm" -o %OutFile% -method CoBaLP2 -rct A1_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RXJ0852-46_radio.ppm" -o %OutFile% -method SCFauto -rct B2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "RXJ0852-46X.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 5 -tolR1 5 -tolR2 15 -tolR3 15 -tolR4 20 -tolR5 20 -tolG0 5 -tolG1 5 -tolG2 15 -tolG3 15 -tolG4 20 -tolG5 20 -tolB0 5 -tolB1 5 -tolB2 15 -tolB3 15 -tolB4 20 -tolB5 20 -log_name %LogFile% >> %res_en%
%EXE% -i "saltnpepper.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Sammellinse.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "satellite.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "saturn.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "save-png-dialog.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "scheimpflug-cameras-for-anterior-segment-tomography-cornea-and-anterior-chamber-422700.ppm" -o %OutFile% -method CoBaLP2 -rct A5_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Schematisierte_Stadtentwicklung_Rostock.svg.ppm" -o %OutFile% -method HXC2aut -rct A5_10 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Schematisierte_Stadtentwicklung_Rostock.svg.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Schematisierte_Stadtentwicklung_Rostock.svg.ppm" -o %OutFile% -method HXC2aut -rct A3_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Schematisierte_Stadtentwicklung_Rostock.svg.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 1 -tolG5 1 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Schematisierte_Stadtentwicklung_Rostock.svg.ppm" -o %OutFile% -method HXC2aut -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Schlammfliege_in_Altlandsberg_1.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -log_name %LogFile% >> %res_en%
%EXE% -i "Schlammfliege_in_Altlandsberg_1.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Schlammfliege_in_Altlandsberg_3.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -log_name %LogFile% >> %res_en%
%EXE% -i "Schlammfliege_in_Altlandsberg_3.ppm" -o %OutFile% -method CoBaLP2 -rct A7_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Schlammfliege_in_Altlandsberg_3.ppm" -o %OutFile% -method CoBaLP2 -rct A4_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "schriftzeichen.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Science_Busters_for_Kids_one_col.ppm" -o %OutFile% -method CoBaLP2 -rct A8_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science_fiction_14.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science_fiction_18.ppm" -o %OutFile% -method CoBaLP2 -rct A9_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science_fiction_7.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science_fiction_9.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science_process_diagram_big.ppm" -o %OutFile% -method SCFauto -rct A1_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Science_Slam.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science_slam_mai_1330963836.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science_teacher.ppm" -o %OutFile% -method SCFauto -rct A1_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science_teacher_2.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "science_thunder.ppm" -o %OutFile% -method SCFauto -rct A2_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Science_Wraps_2010.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science-driver-projekt_cu5818.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science-fair-project-forum-ad.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science-ie1024x768.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science-papas-team.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 1 -histMode kopt -pred none -segWidth 485 -skip_rlc1 1 -postBWT IFC -IFCmax 5 -IFCreset 50 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "science-tweeps-influence-090422.ppm" -o %OutFile% -method SCFauto -rct A2_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ScienceWeek2010_03.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ScinetChartDataVisualization.ppm" -o %OutFile% -method CoBaLP2 -rct RGB -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "SCORE2007_.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Screen-shot-2013-01-24-at-11.42.36-AM.ppm" -o %OutFile% -method HXC2aut -rct A1_10 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Screen-Shot-2015-05-27-at-4.38.19-PM-690x460.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ScreenShot37.ppm" -o %OutFile% -method CoBaLP2 -rct A3_12 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ScreenShot39.ppm" -o %OutFile% -method CoBaLP2 -rct A3_2 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ScummVM_1_0_0.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ScummVM_1_0_0.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "SD77-1.ppm" -o %OutFile% -method CoBaLP2 -rct A1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "sea_dusk.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "seagull.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Second_World_War_Europe_02_1942_de.ppm" -o %OutFile% -method SCFauto -rct A8_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Second_World_War_Europe_11_1939_de.ppm" -o %OutFile% -method SCFauto -rct A6_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Senftenberg.ppm" -o %OutFile% -method SCFauto -rct A2_8 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Sergio_Birga_(2010).ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "server.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -log_name %LogFile% >> %res_en%
%EXE% -i "server.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "server.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "server.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "server.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "sessa_fig3.ppm" -o %OutFile% -method SCFauto -rct A6_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Seth1_2.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Seth2_2.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ShackletonEnduranceCarte.ppm" -o %OutFile% -method SCFauto -rct A2_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "shadow.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "shadow.ppm" -o %OutFile% -method CoBaLP2 -rct A5_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Shengwei95_2.ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "SHHSLogo.ppm" -o %OutFile% -method SCFauto -rct A2_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ship.rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A8_7 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "SHOCKWAVE_.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Short_(finance).ppm" -o %OutFile% -method SCFauto -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "SignedShortLosslessBug_(PNG).ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Sikaku_.ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Sikaku_.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Sikaku_.ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "SimilarMotion.ppm" -o %OutFile% -method HXC2 -rct A1_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 2 -tolG5 1 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Simple_png.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "SistemamundialdelsigloXIII.ppm" -o %OutFile% -method SCFauto -rct A3_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Situacion_Ames.ppm" -o %OutFile% -method SCFauto -rct A1_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Skin.ppm" -o %OutFile% -method TSIP -rct A2_1 -histMode comp -pred ADAPT -blockWidth 25 -blockHeight Med_Paeth_Lin_GAP_CoBaLP -segWidth 1029 -segHeight 25 -interleaving Y_U_V -skip_rlc1 0 -postBWT IFC -IFCmax 271 -IFCreset 4336 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Smilee(2).ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Snarkfit5.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Snarkfit5.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Snarkfit5.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "soft-drinks-7z2.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Sol_1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "soldier_x.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Solis'.ppm" -o %OutFile% -method CoBaLP2 -rct A9_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Spain_(demis).ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Spectroscope_(PSF).ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Spectroscope_(PSF).ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Spectroscopic_mapping_speeds_of_the_SPICA_far-IR-instrument.ppm" -o %OutFile% -method CoBaLP2 -rct A1_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Spherulite_2.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Splenda-povray.ppm" -o %OutFile% -method HXC2 -rct A3_9 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 1 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 522 -tolG5 522 -tolB0 11 -tolB1 11 -tolB2 99 -tolB3 99 -tolB4 522 -tolB5 522 -log_name %LogFile% >> %res_en%
%EXE% -i "Splittings-1.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "spree_250.ppm" -o %OutFile% -method SCF -rct A3_1 -histMode none -pred none -HXC2auto 0 -SCF_prediction 1 -SCF_directional 1 -SCF_stage2tuning 1 -SCF_colourMAP 1 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 14 -tolR3 14 -tolR4 150 -tolR5 152 -tolG0 0 -tolG1 0 -tolG2 12 -tolG3 12 -tolG4 104 -tolG5 104 -tolB0 0 -tolB1 0 -tolB2 13 -tolB3 13 -tolB4 122 -tolB5 122 -log_name %LogFile% >> %res_en%
%EXE% -i "Spring1887.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Squelch_1.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Squelch_3.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "squirrel.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ß.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ss433_natural_colour.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Sshot-1.ppm" -o %OutFile% -method SCFauto -rct A2_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Sta01AUS(vertical).ppm" -o %OutFile% -method SCFauto -rct A5_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "STA60043_.ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "stadtplan-california-academy-of-science-1.ppm" -o %OutFile% -method SCF -rct A2_2 -histMode comp -pred none -HXC2auto 0 -SCF_prediction 1 -SCF_directional 1 -SCF_stage2tuning 0 -SCF_colourMAP 1 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 20 -tolR3 20 -tolR4 40 -tolR5 40 -tolG0 0 -tolG1 0 -tolG2 20 -tolG3 20 -tolG4 40 -tolG5 40 -tolB0 0 -tolB1 0 -tolB2 20 -tolB3 20 -tolB4 40 -tolB5 40 -log_name %LogFile% >> %res_en%
%EXE% -i "stadtplan-museum-of-science-industry-1.ppm" -o %OutFile% -method SCF -rct RGB -histMode comp -pred none -HXC2auto 0 -SCF_prediction 1 -SCF_directional 1 -SCF_stage2tuning 0 -SCF_colourMAP 1 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 23 -tolR3 23 -tolR4 23 -tolR5 23 -tolG0 0 -tolG1 0 -tolG2 13 -tolG3 13 -tolG4 25 -tolG5 25 -tolB0 1 -tolB1 1 -tolB2 5 -tolB3 5 -tolB4 15 -tolB5 15 -log_name %LogFile% >> %res_en%
%EXE% -i "Standing_.ppm" -o %OutFile% -method CoBaLP2 -rct A9_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Statehood_quarters_map_2009.ppm" -o %OutFile% -method SCFauto -rct A9_4 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Statehood_quarters_map_2009.ppm" -o %OutFile% -method SCFauto -rct B3_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Statehood_quarters_map_2009.ppm" -o %OutFile% -method SCFauto -rct B3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Statincomparison_3.ppm" -o %OutFile% -method HXC2 -rct A1_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Statinfamily.3.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "steamtrain.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Stèle_gravée,_Fourvière_2.ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Stirling_Beta-Typ.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "stockschütze.ppm" -o %OutFile% -method CoBaLP2 -rct B1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "stone.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "stone-i9sv.ppm" -o %OutFile% -method CoBaLP2 -rct A6_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "stones-cairns-76.ppm" -o %OutFile% -method CoBaLP2 -rct A9_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "stones-dx68.ppm" -o %OutFile% -method CoBaLP2 -rct A6_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "stones-g7x9.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "stones-t8c3.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Stracture_.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Strand_.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "STTr.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "sub-4.ppm" -o %OutFile% -method CoBaLP2 -rct B1_1 -log_name %LogFile% >> %res_en%
%EXE% -i "sub-4.ppm" -o %OutFile% -method CoBaLP2 -rct B1_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "SUFig-56.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "SUFig-57.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "sunflower.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "sunset.ppm" -o %OutFile% -method SCFauto -rct B1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "sunset3.ppm" -o %OutFile% -method CoBaLP2 -rct B1_1 -log_name %LogFile% >> %res_en%
%EXE% -i "sunset3.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "supersexygirls-pack-2.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Supertux0_1_3.ppm" -o %OutFile% -method HXC2 -rct A3_1 -histMode comp -pred MEDPAETH -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 254 -tolR3 254 -tolR4 254 -tolR5 254 -tolG0 0 -tolG1 0 -tolG2 15 -tolG3 15 -tolG4 442 -tolG5 442 -tolB0 0 -tolB1 0 -tolB2 13 -tolB3 13 -tolB4 505 -tolB5 505 -log_name %LogFile% >> %res_en%
%EXE% -i "Supertuxkart_0.6.2.ppm" -o %OutFile% -method LOCO -rct A3_7 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "SXI_20060706_151118162_AA_13_first.ppm" -o %OutFile% -method HXC2 -rct A3_11 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 110 -tolR3 110 -tolR4 220 -tolR5 220 -tolG0 0 -tolG1 0 -tolG2 110 -tolG3 110 -tolG4 220 -tolG5 220 -tolB0 0 -tolB1 0 -tolB2 110 -tolB3 110 -tolB4 220 -tolB5 220 -log_name %LogFile% >> %res_en%
%EXE% -i "SzJO_1.ppm" -o %OutFile% -method HXC2 -rct A4_2 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 3 -tolR3 3 -tolR4 222 -tolR5 222 -tolG0 0 -tolG1 0 -tolG2 3 -tolG3 3 -tolG4 222 -tolG5 222 -tolB0 0 -tolB1 0 -tolB2 3 -tolB3 3 -tolB4 222 -tolB5 222 -log_name %LogFile% >> %res_en%
%EXE% -i "T1_01_c201401.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "T1_02_c124101.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "T1_04_c431204.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "T1_05_c431306.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "T6_01_19o.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Taiwan_road_sign_Art026.2.ppm" -o %OutFile% -method HXC2 -rct A3_3 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 21 -tolR3 21 -tolR4 53 -tolR5 53 -tolG0 0 -tolG1 0 -tolG2 38 -tolG3 38 -tolG4 71 -tolG5 71 -tolB0 0 -tolB1 0 -tolB2 4 -tolB3 4 -tolB4 31 -tolB5 31 -log_name %LogFile% >> %res_en%
%EXE% -i "Taiwan_road_sign_Art027.2.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Taiwan_road_sign_Art058.1.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "tajmahal.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Talinslstl_(2).ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Talinslstl_(6).ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Talinslstl_(7).ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "tangoIcons.ppm" -o %OutFile% -method SCFauto -rct A4_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tatra_mountains_western_side_1_desc.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tatra_mountains_western_side_2_desc.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -log_name %LogFile% >> %res_en%
%EXE% -i "Tatra_mountains_western_side_2_desc.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tectonic_plates.ppm" -o %OutFile% -method SCFauto -rct A8_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tectonic_plates.ppm" -o %OutFile% -method HXC2aut -rct A8_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tectonic_plates.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tectonic_plates.ppm" -o %OutFile% -method HXC2aut -rct A6_8 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tectonic_plates.ppm" -o %OutFile% -method HXC2aut -rct A8_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tectonic_plates.ppm" -o %OutFile% -method HXC2aut -rct A2_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tectonic_plates.ppm" -o %OutFile% -method HXC2aut -rct A8_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Teenagers_.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -log_name %LogFile% >> %res_en%
%EXE% -i "Teenagers_.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TentePost1910_.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tentimage_2.ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Teste17.2.ppm" -o %OutFile% -method CoBaLP2 -rct A3_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "text.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 1 -tolR2 15 -tolR3 15 -tolR4 15 -tolR5 15 -tolG0 0 -tolG1 0 -tolG2 15 -tolG3 15 -tolG4 15 -tolG5 15 -tolB0 0 -tolB1 0 -tolB2 15 -tolB3 15 -tolB4 15 -tolB5 15 -log_name %LogFile% >> %res_en%
%EXE% -i "texture-009.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TF3D_Landscape.ppm" -o %OutFile% -method SCFauto -rct A3_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "The.Matrix.glmatrix.1.ppm" -o %OutFile% -method CoBaLP2 -rct A6_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "The_Byrds_(1967).ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "The_Earth_seen_from_Apollo_17.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "The_Earth_seen_from_Apollo_17.ppm" -o %OutFile% -method CoBaLP2 -rct A2_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "The_Earth_seen_from_Apollo_17.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 0 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 1 -tolG1 1 -tolG2 12 -tolG3 12 -tolG4 16 -tolG5 16 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "The+Saturdays+Better+Quality+in+PNG.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "The+Veronicas+PNG.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TheJabberwocky.ppm" -o %OutFile% -method HXC2aut -rct 0 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TheJabberwocky.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TheMatrix_glmatrix_2.ppm" -o %OutFile% -method CoBaLP2 -rct A6_9 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ThomsonsPolly_page09.ppm" -o %OutFile% -method HXC2aut -rct 0 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "ThomsonsPolly_page09.ppm" -o %OutFile% -method HXC2aut -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Thracianocree1_.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TIC_Wallpaper001.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "tiger.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Tim_Yap_2.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Timezones2008.ppm" -o %OutFile% -method LOCO -rct A8_1 -log_name %LogFile% >> %res_en%
%EXE% -i "Timezones2008.ppm" -o %OutFile% -method HXC2aut -rct A2_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Timezones2008.ppm" -o %OutFile% -method LOCO -rct A3_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Timezones2008.ppm" -o %OutFile% -method LOCO -rct A5_4 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Timezones2008.ppm" -o %OutFile% -method LOCO -rct RGB -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Tmfaseiiiv4a_(1).ppm" -o %OutFile% -method LOCO -rct RGB -log_name %LogFile% >> %res_en%
%EXE% -i "Tmfaseiiiv4a_(1).ppm" -o %OutFile% -method LOCO -rct A1_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Tmfaseiiiv4a_(1).ppm" -o %OutFile% -method LOCO -rct A2_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Tmfaseiiiv4a_(1).ppm" -o %OutFile% -method LOCO -rct A2_3 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Tmfaseiiiv4a_(1).ppm" -o %OutFile% -method LOCO -rct A1_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Tmfaseiiiv4a_(1).ppm" -o %OutFile% -method LOCO -rct A2_6 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Tmfaseiiiv4a_(1).ppm" -o %OutFile% -method LOCO -rct A1_3 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Tmp-1245434728.1.ppm" -o %OutFile% -method BPC -rct 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Tmp-1245434728.2.ppm" -o %OutFile% -method BPC -rct 0 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "TNCCA.ppm" -o %OutFile% -method SCF -rct A1_10 -histMode comp -pred none -HXC2auto 0 -SCF_prediction 1 -SCF_directional 1 -SCF_stage2tuning 0 -SCF_colourMAP 1 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 255 -tolR5 255 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 255 -tolG5 255 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 255 -tolB5 255 -log_name %LogFile% >> %res_en%
%EXE% -i "Toboggan_(PSF).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 15 -tolR1 15 -tolR2 63 -tolR3 63 -tolR4 63 -tolR5 63 -log_name %LogFile% >> %res_en%
%EXE% -i "tomography-1-PN1.ppm" -o %OutFile% -method LOCO -rct RGB -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "topcategorychart.ppm" -o %OutFile% -method HXC2aut -rct A3_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Totem3_2_0.ppm" -o %OutFile% -method CoBaLP2 -rct A8_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tour_de_France_2012_-_Etappe_7.ppm" -o %OutFile% -method SCFauto -rct A3_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TpyCoS-HOMO-glsl.ppm" -o %OutFile% -method CoBaLP2 -rct A2_3 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tractrix-sketch-lettered-colour.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tradition_(10).ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 18 -tolR3 18 -tolR4 21 -tolR5 21 -log_name %LogFile% >> %res_en%
%EXE% -i "train_1.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "train_2.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Transform-.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TreatyOfSevres_(corrected).ppm" -o %OutFile% -method SCFauto -rct B1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tree_.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "tree-32.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "tree-794.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "tree-flowers-u6v.ppm" -o %OutFile% -method CoBaLP2 -rct A2_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "tree-flowers-u6v_b01_U.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "tree-flowers-u6v_b01_V.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "truck_01.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TuckerLemmaDiagram.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Tunnel.2.ppm" -o %OutFile% -method BPC -rct A5_11 -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Tunnel.3.ppm" -o %OutFile% -method SCFauto -rct A8_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "tutorial001.ppm" -o %OutFile% -method BPC -rct A3_4 -histMode none -pred ADAPT -blockWidth 1981 -blockHeight 0 -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "tutorial010.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "tux-agafix.ppm" -o %OutFile% -method HXC2 -rct A2_4 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 2 -tolR3 2 -tolR4 110 -tolR5 110 -tolG0 0 -tolG1 0 -tolG2 20 -tolG3 20 -tolG4 110 -tolG5 110 -tolB0 0 -tolB1 0 -tolB2 20 -tolB3 20 -tolB4 110 -tolB5 110 -log_name %LogFile% >> %res_en%
%EXE% -i "TW-Art031.1.ppm" -o %OutFile% -method HXC2aut -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TW-Art063.2.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TW-Art063.3.ppm" -o %OutFile% -method SCFauto -rct A3_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TW-Art068.2.ppm" -o %OutFile% -method SCFauto -rct A3_8 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TW-Art069.6.ppm" -o %OutFile% -method SCFauto -rct A1_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TW-Art080.1.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 14 -tolR3 14 -tolR4 105 -tolR5 105 -tolG0 0 -tolG1 0 -tolG2 27 -tolG3 27 -tolG4 41 -tolG5 41 -tolB0 0 -tolB1 0 -tolB2 30 -tolB3 30 -tolB4 62 -tolB5 62 -log_name %LogFile% >> %res_en%
%EXE% -i "TW-Art097.1.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TW-Art105.4.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "TW-Art135.2.ppm" -o %OutFile% -method SCFauto -rct A3_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ubicación...ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Ubicación_Córdoba_2.ppm" -o %OutFile% -method SCFauto -rct A2_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Uebersicht-RD.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "UKdo_4.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "UKdo_7.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Umbrello_1.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Umkehrregel_1.ppm" -o %OutFile% -method HXC2 -rct A3_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 1 -tolR1 1 -tolR2 1 -tolR3 1 -tolR4 1 -tolR5 1 -tolG0 2 -tolG1 1 -tolG2 1 -tolG3 1 -tolG4 2 -tolG5 1 -tolB0 1 -tolB1 1 -tolB2 1 -tolB3 1 -tolB4 1 -tolB5 1 -log_name %LogFile% >> %res_en%
%EXE% -i "United_Kingdom_civil_parishes,_communities_and_council_areas.ppm" -o %OutFile% -method LOCO -rct A3_1 -log_name %LogFile% >> %res_en%
%EXE% -i "United_Kingdom_civil_parishes,_communities_and_council_areas.ppm" -o %OutFile% -method LOCO -rct A1_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "United_Kingdom_civil_parishes,_communities_and_council_areas.ppm" -o %OutFile% -method LOCO -rct RGB -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "United_Kingdom_civil_parishes,_communities_and_council_areas.ppm" -o %OutFile% -method LOCO -rct A1_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Uniteddestinations.ppm" -o %OutFile% -method SCFauto -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Universe_expansion.ppm" -o %OutFile% -method SCFauto -rct A2_3 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Univert..ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Uridine_to_pseudouridine.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Uvulopalatopharyngoplasty..ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Valerio_Adami_(1995).ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Vegetation.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Vela_frs.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Vela_Jr.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "vela_jr_rosat.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "VELAnalysis_1.3.ppm" -o %OutFile% -method BPC -rct RGB -histMode comp -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Vertetimi2.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 27 -tolR3 27 -tolR4 27 -tolR5 27 -log_name %LogFile% >> %res_en%
%EXE% -i "VMGdwfttw_2.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Volvocales.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "wallpaper_oceanfreight_1280x960.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "wallpaper_railfreight_1280x960.ppm" -o %OutFile% -method CoBaLP2 -rct A3_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Wanakura-2.ppm" -o %OutFile% -method CoBaLP2 -rct RGB -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Warship_2.ppm" -o %OutFile% -method HXC2 -rct 0 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 88 -tolR3 88 -tolR4 255 -tolR5 255 -log_name %LogFile% >> %res_en%
%EXE% -i "Wasserturm-1.ppm" -o %OutFile% -method CoBaLP2 -rct A9_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "water-08.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "water-dx3d.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Waterfall_1.ppm" -o %OutFile% -method SCFauto -rct A1_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "water-ual1.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "water-uv5k.ppm" -o %OutFile% -method CoBaLP2 -rct A8_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Wavelength.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WBK_V_(V1).ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "We_are_anonymous.ppm" -o %OutFile% -method LOCO -rct A7_1 -log_name %LogFile% >> %res_en%
%EXE% -i "We_are_anonymous.ppm" -o %OutFile% -method LOCO -rct A1_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "We_are_anonymous.ppm" -o %OutFile% -method LOCO -rct A2_3 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Web3_4_1.ppm" -o %OutFile% -method CoBaLP2 -rct A4_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WEBDESIGNER_ISSUE_100.ppm" -o %OutFile% -method HXC2aut -rct A9_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Weylandknot.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WFCAM_JHK_colour-composite_image.ppm" -o %OutFile% -method CoBaLP2 -rct A9_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "what.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WIKI_1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Wiki_graphic_2.ppm" -o %OutFile% -method LOCO -rct A7_10 -log_name %LogFile% >> %res_en%
%EXE% -i "Wiki_graphic_2.ppm" -o %OutFile% -method LOCO -rct A3_12 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Wiki_graphic_2.ppm" -o %OutFile% -method LOCO -rct A3_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Wiki_graphic_2.ppm" -o %OutFile% -method LOCO -rct A3_2 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Wiki_graphic_3.ppm" -o %OutFile% -method CoBaLP2 -rct A3_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Wikimic2.ppm" -o %OutFile% -method HXC2 -rct A3_12 -histMode comp -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 30 -tolR3 30 -tolR4 30 -tolR5 30 -tolG0 1 -tolG1 1 -tolG2 8 -tolG3 8 -tolG4 8 -tolG5 8 -tolB0 0 -tolB1 0 -tolB2 18 -tolB3 18 -tolB4 18 -tolB5 18 -log_name %LogFile% >> %res_en%
%EXE% -i "Wikiminatlas2_0.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Wikipic3_1.ppm" -o %OutFile% -method SCFauto -rct 0 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Wikisangamolsavam_5.ppm" -o %OutFile% -method SCFauto -rct A2_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Wikispecies-logo.ppm" -o %OutFile% -method TSIP -Idx 1 -Idx_sorting 0 -histMode kopt -pred none -segWidth 1103 -skip_rlc1 1 -postBWT IFC -IFCmax 5 -IFCreset 15 -skip_prec 0 -RLC2_zeros 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WiktionaryMasry.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "winaw.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Winboard_4.2.7.ppm" -o %OutFile% -method SCFauto -rct A2_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Winboard_4.2.7.ppm" -o %OutFile% -method SCFauto -rct A3_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Winboard_4.2.7.ppm" -o %OutFile% -method SCFauto -rct RGB -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Window_function_(rectangular).ppm" -o %OutFile% -method SCFauto -rct A2_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Window_function_(rectangular).ppm" -o %OutFile% -method HXC2 -rct A3_11 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Window_function_(rectangular).ppm" -o %OutFile% -method HXC2aut -rct A3_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "window1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "window1.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "window1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "window1.ppm" -o %OutFile% -method CoBaLP2 -rct A5_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "window1.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Windows-Live-Writer-Fundamentals-of-Visual-Studio-LightSwitc_D0C9-image_7f98b789-102a-4261-bd5e-c9cf0e62132a.ppm" -o %OutFile% -method HXC2aut -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "winkler-gnu-blue.ppm" -o %OutFile% -method HXC2aut -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WMAP_2008.ppm" -o %OutFile% -method HXC2aut -rct A3_9 -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WMAP_2008.ppm" -o %OutFile% -method HXC2aut -rct A2_4 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WMAP_2008.ppm" -o %OutFile% -method HXC2aut -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WOBIB_140.ppm" -o %OutFile% -method SCF -rct RGB -histMode comp -pred none -HXC2auto 0 -SCF_prediction 1 -SCF_directional 1 -SCF_stage2tuning 0 -SCF_colourMAP 1 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "wolf.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_grey.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_grey.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_rgb.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A6_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A7_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A9_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_rgb.ppm" -o %OutFile% -method CoBaLP2 -rct A8_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_rgb_U.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_rgb_U.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_rgb_V.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "woman_rgb_V.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "woodgrove.ppm" -o %OutFile% -method CoBaLP2 -rct A3_11 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WOSMWorldmap3.ppm" -o %OutFile% -method SCFauto -rct A2_12 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Wotská_orlice.ppm" -o %OutFile% -method HXC2aut -rct B2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Wpvs_6.ppm" -o %OutFile% -method HXC2 -rct RGB -histMode none -pred none -HXC2auto 0 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "WritingSystemsOfTheWorld.ppm" -o %OutFile% -method SCFauto -rct A6_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WritingSystemsOfTheWorld.ppm" -o %OutFile% -method HXC2aut -rct A8_6 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "WritingSystemsOfTheWorld.ppm" -o %OutFile% -method HXC2 -rct A6_6 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 1 -tolR0 0 -tolR1 0 -tolR2 5 -tolR3 5 -tolR4 222 -tolR5 222 -tolG0 0 -tolG1 0 -tolG2 5 -tolG3 5 -tolG4 222 -tolG5 222 -tolB0 0 -tolB1 0 -tolB2 5 -tolB3 5 -tolB4 222 -tolB5 222 -log_name %LogFile% >> %res_en%
%EXE% -i "X.ppm" -o %OutFile% -method HXC2 -rct A2_1 -histMode none -pred none -HXC2auto 0 -HXC2increasefac 4 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 0 -tolR5 0 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 0 -tolG5 0 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 0 -tolB5 0 -log_name %LogFile% >> %res_en%
%EXE% -i "XchatScreenshot2_6_6.ppm" -o %OutFile% -method SCFauto -rct A2_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Xcode_Shortcuts.ppm" -o %OutFile% -method SCF -rct A2_7 -HXC2auto 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Xcode_Shortcuts_B_W.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Xcode_Shortcuts_B_W.ppm" -o %OutFile% -method SCFauto -rct A3_9 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Xcode_Shortcuts_B_W.ppm" -o %OutFile% -method SCFauto -rct A1_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Xcode_Shortcuts.ppm" -o %OutFile% -method SCF -rct A2_10 -histMode comp -pred none -HXC2auto 0 -SCF_prediction 1 -SCF_directional 1 -SCF_stage2tuning 1 -SCF_colourMAP 1 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 510 -tolR5 510 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 510 -tolG5 510 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 510 -tolB5 510 -log_name %LogFile% >> %res_en%
%EXE% -i "Xcode_Shortcuts.ppm" -o %OutFile% -method SCF -rct A2_1 -histMode comp -pred none -HXC2auto 0 -SCF_prediction 1 -SCF_directional 1 -SCF_stage2tuning 1 -SCF_colourMAP 1 -HXC2increasefac 3 -tolR0 0 -tolR1 0 -tolR2 0 -tolR3 0 -tolR4 510 -tolR5 510 -tolG0 0 -tolG1 0 -tolG2 0 -tolG3 0 -tolG4 510 -tolG5 510 -tolB0 0 -tolB1 0 -tolB2 0 -tolB3 0 -tolB4 510 -tolB5 510 -log_name %LogFile% >> %res_en%
%EXE% -i "xray_10bit.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -log_name %LogFile% >> %res_en%
%EXE% -i "xray_10bit.ppm" -o %OutFile% -method CoBaLP2 -rct 0 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "xspecs4_main.ppm" -o %OutFile% -method CoBaLP2 -rct A7_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "XXY+X.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "yahoo.ppm" -o %OutFile% -method SCFauto -rct A2_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Yahya_1_1-5.ppm" -o %OutFile% -method SCFauto -rct A3_1 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Yak-141_painting_scheme_(48-2,_1991).ppm" -o %OutFile% -method LOCO -rct A3_1 -log_name %LogFile% >> %res_en%
%EXE% -i "Yak-141_painting_scheme_(48-2,_1991).ppm" -o %OutFile% -method LOCO -rct A1_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Yak-141_painting_scheme_(48-2,_1991).ppm" -o %OutFile% -method LOCO -rct PEI09 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Yak-141_painting_scheme_(48-2,_1991).ppm" -o %OutFile% -method LOCO -rct A1_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Yak-141_painting_scheme_(48-3_in_museum).ppm" -o %OutFile% -method LOCO -rct A1_1 -log_name %LogFile% >> %res_en%
%EXE% -i "Yak-141_painting_scheme_(48-3_in_museum).ppm" -o %OutFile% -method LOCO -rct A9_6 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Yak-141_painting_scheme_(48-3_in_museum).ppm" -o %OutFile% -method LOCO -rct A1_6 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Yak-141_painting_scheme_(48-3_in_museum).ppm" -o %OutFile% -method LOCO -rct A3_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Yak-141_painting_scheme_(48-3_in_museum).ppm" -o %OutFile% -method LOCO -rct A3_6 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Yak-141_painting_scheme_(48-3_in_museum).ppm" -o %OutFile% -method LOCO -rct A3_1 -histMode comp -pred MED -log_name %LogFile% >> %res_en%
%EXE% -i "Yang.ppm" -o %OutFile% -method HXC2aut -rct A1_11 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Yezidi_Man.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Yezidi_Man-2.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Yezidi_Woman.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Yield.ppm" -o %OutFile% -method BPC -Idx 1 -Idx_sorting 0 -histMode none -pred none -sFlag 1 -pFlag 1 -cFlag 0 -log_name %LogFile% >> %res_en%
%EXE% -i "Z_V.ppm" -o %OutFile% -method CoBaLP2 -rct A2_1 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Zambals_3.ppm" -o %OutFile% -method CoBaLP2 -rct A4_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "zebrafisch.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Zoning2_.ppm" -o %OutFile% -method SCFauto -rct A3_7 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "zoo-016.ppm" -o %OutFile% -method CoBaLP2 -rct A8_4 -histMode comp -sT 1 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Z-scheme_(cs).ppm" -o %OutFile% -method SCFauto -rct A1_2 -histMode comp -pred none -HXC2auto 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Zweiter_Paderborner_Science_Slam_Poster.ppm" -o %OutFile% -method CoBaLP2 -rct A1_2 -log_name %LogFile% >> %res_en%
%EXE% -i "Zweiter_Paderborner_Science_Slam_Poster.ppm" -o %OutFile% -method CoBaLP2 -rct A7_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Zweiter_Paderborner_Science_Slam_Poster.ppm" -o %OutFile% -method CoBaLP2 -rct A1_4 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Zweiter_Paderborner_Science_Slam_Poster.ppm" -o %OutFile% -method CoBaLP2 -rct PEI09 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
%EXE% -i "Zweiter_Paderborner_Science_Slam_Poster.ppm" -o %OutFile% -method CoBaLP2 -rct A4_10 -histMode comp -sT 0 -sA 0 -sB 0 -constLines 1 -log_name %LogFile% >> %res_en%
if !flag! == 1 (

echo end of file list

)

Rem go back to initial directory
cd %currentDir%

pause

Rem @echo on
