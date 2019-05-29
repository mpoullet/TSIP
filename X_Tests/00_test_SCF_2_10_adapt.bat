@echo off 
REM damit Zeit aktuakisiert wird:
SetLocal EnableDelayedExpansion

set ENCODER="..\bin\TSIPcoder_2_10.exe"
set DECODER="..\bin\TSIPcoder_2_10.exe"
set PSNR="PSNR_C.exe"

set DIR=C:\Users\Strutz\Documents\Texte\Paper\2016-IEEE-JETCAS-SCF\Investigations\Testimages\PPM_trimmed

set res="01_SCF_2_10.txt"
set Outfile="tmp210.tsip"
set Logfile="log_SCF_2_10_auto.txt"
set Recofile="reco210.ppm"

set flag=1
REM 
for  %%I in (%DIR%\*.ppm) do (

REM   for  %%I in (..\Investigations\Testimages\PPM_trimmed\global-multi-scree_560x688.ppm   ) do (

echo # %%I
echo %DIR%\76167_600x496.ppm
REM start with this images
 if "%%I" == "%DIR%\76167_600x496.ppm" (
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


REM     %DECODER%  -i %Outfile% -o %Recofile%   >> %res% 

echo Finished Decoder !time:~0,8!
echo Finished Decoder !time:~0,8! >> %res%

REM  %PSNR% -i1  %%I -i2 %Recofile%  -p  >> %res%


REM     del %Recofile%

   )
  )
  echo    #  %%I >> %res%

)




pause
@echo on