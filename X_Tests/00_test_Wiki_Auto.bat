@echo off

setlocal enabledelayedexpansion


REM 
set EXE="..\bin\TSIPcoder_1_35a.exe"
REM set EXE="..\bin_debug\TSIPcoder.exe"
set PSNR="PSNR_C.exe"
set OutFile=tmpAutoW.tsip
set LogFile=log_TSIP_Wiki_Auto.txt
set res_en="res_Auto_Wiki.txt"

REM -method TSIP  -pred ADAPT -skip_rlc1 0 -RLC2_zeros 0 -entropy_coding 0 -predictors 111111 -blockWidth 100 -blockHeight 100 -o tmp.tsip -i C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\Farbkreis_Itten_1961.ppm 

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

REM -o  tmp.tsip -method TSIP -Idx 1 -method HXC -i C:\Users\Strutz\Documents\Daten\science\5colors.png  
REM -o  tmp.tsip -method TSIP -Idx 0 -rct 1 -method HXC -i C:\Users\Strutz\Documents\Daten\science\5colors.png  

REM for  %%I in (C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\5colors.png ) do (
REM for  %%I in (C:\Users\Strutz\Documents\Daten\science\apple-world-aquagraph-16074.ppm C:\Users\Strutz\Documents\Daten\science\408px-Killersudoku_color.ppm) do (
REM for  %%I in (C:\Users\Strutz\Documents\Daten\science\5colors.ppm ) do (
REM for  %%I in (C:\Users\Strutz\Documents\Daten\science\ScienceWeek2010_03.png 
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\*.png                     
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\*.ppm                     
REM  C:\Users\Strutz\Documents\Daten\LCIC\*.ppm                     
REM  C:\Users\Strutz\Documents\Daten\IEETA.PT_ap\*.ppm                     
REM  C:\Users\Strutz\Documents\Daten\PWC-Corpus\*.ppm                     
REM  C:\Users\Strutz\Documents\Daten\TZ5_PNG\*.png                     
REM  C:\Users\Strutz\Documents\Daten\TZ5_PPM\*.ppm                     
REM  C:\Users\Strutz\Documents\Daten\science\*.png                     
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\Additional\*.png                     
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\Additional\*.pgm                     
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\Cone-response.ppm 
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\dumper.ppm 
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\europa_karte_de_PNGOUT.ppm 
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\noir-diabolik-guitar-hommess_design.png
REM  C:\Users\Strutz\Documents\Daten\LOCO-I_colour2Test\248080202_731e632c11_o.png                    
REM  C:\Users\Strutz\Documents\Daten\science\F1_medium.png
REM  C:\Users\Strutz\Documents\Daten\science\I-love-science1024x768.png
REM  C:\Users\Strutz\Documents\Daten\science\convert-pdf-to-bmp.png                    
REM  C:\Users\Strutz\Documents\Daten\science\Portal_2_Potatoe_Science_Kit.png                    
REM  C:\Users\Strutz\Documents\Daten\science\Alvarez_science_path.png                    
REM  C:\Users\Strutz\Documents\Daten\science\ncat_fig7_8.png                    
REM  C:\Users\Strutz\Documents\Daten\science\cov-journal-science-prev2-med.png                    
REM  C:\Users\Strutz\Documents\Daten\science\TNCCA.ppm 
REM  C:\Users\Strutz\Documents\Daten\science\ss433_natural_colour.ppm 
REM  C:\Users\Strutz\Documents\Daten\Science\hyades_mosaic.png 
REM  C:\Users\Strutz\Documents\Daten\science\stadtplan-museum-of-science-industry-1.ppm 
REM  C:\Users\Strutz\Documents\Daten\CT2\science\sessa_fig3.png                     
REM  C:\Users\Strutz\Documents\Daten\Maps-Joensuu\T6_01_19o.png
REM  C:\Users\Strutz\Documents\Daten\PWC-Corpus\arial.ppm
REM  C:\Users\Strutz\Documents\Daten\WikimediaCommons\793px-Fig.2.3.png

REM colour_space.ppm

REM ) do (



REM echo #Results > %res_en%
REM echo #Results > %res_de%
set flag=0


REM ##################### Main Loop ##############################
for  %%I in (                               
	C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\*.ppm       
	C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\Not-HXC\*.ppm       
	C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\LargeImages\*.ppm       
) do (

REM for /f "usebackq delims=" %%I in (HXC_images_TestSet.txt) do (
REM set image=%%L

echo %%I

REM start with this images
 if "%%I" == "C:\Users\Strutz\Documents\Daten\CT2\WikimediaCommons\PPM\Thracianocree1_.ppm" (
 	set flag=1
  echo !flag!
 )

REM 
if !flag! == 1 (


REM  pngtopnm.exe %%I > tmp.ppm

 %EXE% -i %%I -o %OutFile%  -log_name %LogFile% >> %res_en%

REM  %EXE% -i tmp4.tsip -o recoS.ppm  >> %res_de%
REM  %PSNR% -i1 %%I -i2 recoS.ppm  -p  >> %res_en%

 
REM erase recoS.ppm
REM 
)



REM ##### end of main loop #########
)

pause

REM@echo on