@echo off 
REM damit Zeit aktuakisiert wird:
SetLocal EnableDelayedExpansion

set ENCODER="..\bin\TSIPcoder_2_12a.exe"
set DECODER="..\bin\TSIPcoder_2_12a.exe"
set PSNR="PSNR_C.exe"

set DIR=C:\Users\Strutz\Documents\Texte\Paper\2016-IEEE-JETCAS-SCF\Investigations\Testimages\PPM_trimmed
REM 
set DIR=C:\Users\Strutz\Documents\Texte\Paper\2016-IEEE-JETCAS-SCF\Investigations\Testimages\SCC_Testsequence\PPM\RGB

set res="01_SCF_2_12aMODE.txt"
set Outfile="tmp212aMODE.tsip"
set Logfile="log_SCF_2_12aMODE.txt"
set Recofile="reco212aMODE.ppm"

set flag=1
REM for  %%I in (%DIR%\*.ppm) do (

REM   for  %%I in (%DIR%\chrome-extension_472x264.ppm   ) do (
  for  %%I in (%DIR%\sc_map_1280x720_60_8bit_rgb.ppm   ) do (

echo # %%I
REM echo %DIR%\76167_600x496.ppm
REM start with this images
 if "%%I" == "%DIR%\xxx.ppm" (
 	set flag=1
  echo !flag!
 )

echo %%I
 if !flag! == 1 (


echo # %%I >> %res%



echo Start Encoder: !time:~0,8!
echo Start Encoder: !time:~0,8! >> %res%


   %ENCODER% -i %%I  -o %Outfile%  -method SCF -HXC2auto 1 -histmode comp -pred none -tilewidth 99999 -tileheight 999999 -log_name %Logfile% >> %res% 

echo Start Decoder: !time:~0,8!
echo Start Decoder: !time:~0,8! >> %res%


REM   
  %DECODER%  -i %Outfile% -o %Recofile%   >> %res% 

echo Finished Decoder !time:~0,8!
echo Finished Decoder !time:~0,8! >> %res%

REM 
 %PSNR% -i1  %%I -i2 %Recofile%  -p  >> %res%


REM  
   del %Recofile%

   )
  )
  echo    #  %%I >> %res%

)



pause
@echo on