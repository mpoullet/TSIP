@echo off

setlocal enabledelayedexpansion


REM 
set EXE2="..\bin\TSIPcoder_2_15a.exe"
REM set EXE="..\bin_debug\TSIPcoder.exe"
set PSNR="PSNR_C.exe"

set OutFile2=tmpW3.tsip
set LogFile2=log_TSIP_W15.txt
set res_en2="res_test_W3.txt"

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





REM 
echo #Results >> %res_en2%
set flag=1


REM ##################### Main Loop ##############################
goto start
:start
for  %%I in (
      C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\*.pngXX
      C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\*.ppmXX
      C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\CoBaLP2\*.pngXX
      C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\CoBaLP2\*.ppmXX
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\2-Methylpentane_1.ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\267px-Surcoat_(PSF).ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\387px-Microburil_2.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\582px-Pfeffer_Osmotische_Untersuchungen-5-2.ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\615px-Pannier_(PSF).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\CoBaLP2\609px-Victor_(steamboat).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\620px-Dynamo_(PSF).ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\640px-ETO..ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\640px-IJ_(letter).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\649px-Sponge_(PSF).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\666px-Tandem_(PSF).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\800px-Piazza_(PSF).ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\800px-C.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\800px-Diap3.1.ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\800px-Thorax_(PSF).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\857px-Frame_(PSF).ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\1024px-ComputerMemoryHierarchy.ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\1024px-Hemispheres_(PSF).ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\1024px-Table-3.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Acting...ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Adminpedia.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Benzaldehyde.ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Bermudadreieck_(Skizze).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Boom_(PSF).ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Bpmn-1.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\C32_7.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Common-whipping-binding-3.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\CopticLetters.ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Cuckoo_(PSF).ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\DessousElegants1905n8p120.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\DessousElegants1905n8pX.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\DessousElegants1905n8pX.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\DessousElegants1905n8p134.ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\D3D_Pipeline_(en).ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Dresden_gemarkungen.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\CoBaLP2\Eclairage.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Figured_Bass_Inversions_1_and_2.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Forthstack1_5.ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Friedrich_condenser_(2).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Galeodes_(Lydekker).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Gallinula_chloropus(PSF).ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Grey_720-360.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Gocursus-dia1-2.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Hist-gamma4.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\ISN_200_ARB_2006_transcript_Pg_26.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Indonesia_Raya_-_Sin_Po_Wekelijksche_Editie_-_10_Nov_1928.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Irene_Adler.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\ISN_10020_CSRT_2007_transcript_Pg_4.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\ISN_10020_CSRT_2007_transcript_Pg_2.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\KlausWeber-USPatent3518014-Fig1-3.ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Map_of_Allegheny_County,_Pennsylvania.ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\MD82v1.0.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Moonalphabet.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Panic10.6.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Pig...ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Rangoli....ppm
	C:\Users\Strutz\Documents\Daten\CT2\Grey\ruler.ppm
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\SignedShortLosslessBug_(PNG).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Spectroscope_(PSF).ppm"
	"C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\sz.ppm"
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Taiwan_road_sign_Art027.2.ppm
	C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\Wikipic3_1.ppm
) do (

echo "%%I"

REM start with this images
 if "%%I" == "C:\Users\Strutz\Documents\Daten\CT2\777-Contest-PPM\.ppm" (
 	set flag=1
  echo !flag!
 )

 if !flag! == 1 (


 echo .
 echo   "%%I"   >> %res_en2%


REM  for /L %%F in (0,1,2) do (
set PARAM=-method scf -HXC2auto 1 -histmode comp
set PARAM=-method scf -tolR0 10 -tolR1 10 -tolR2 40 -tolR3 40 -tolR4 60 -tolR5 60 -HXC2increasefac 1
set PARAM=-method scf -tolR0 0 -tolR1 0 -tolR2 110 -tolR3 110 -tolR4 220 -tolR5 220 -HXC2increasefac 1
set PARAM=-method bpc
    %EXE2% -i "%%I" -o %OutFile2% !PARAM! -log_name %LogFile2% >> %res_en2% 
	
REM     %EXE% -i tmp1.tsip -o reco.png  >> %res_de%
REM     %PSNR% -i1 "%%I" -i2 reco.png  -p  >> %res_en%

REM    del reco.png
REM  )
 )

REM ##### end of main loop #########
)

pause

REM
@echo on