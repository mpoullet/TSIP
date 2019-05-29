@echo off 
REM damit Zeit aktuakisiert wird:
SetLocal EnableDelayedExpansion

set ENCODER="..\bin\TSIPcoder_2_10.exe"
set DECODER="..\bin\TSIPcoder_2_10.exe"
set PSNR="PSNR_C.exe"


set res="01_SCF_2_10_set2.txt"
set Outfile="tmp210_set2.tsip"
set Logfile="log_SCF_2_10_auto_set2.txt"
set Recofile="reco210_set2.ppm"

set DIR=C:\Users\Strutz\Documents\Texte\Paper\2016-IEEE-JETCAS-SCF\Investigations\Testimages\SCC_Testsequence\PPM\RGB

set flag=1
REM 
for  %%I in (%DIR%\*.ppm) do (
REM for  %%I in (..\Investigations\Testimages\SCC_Testsequence\PPM\RGB\sc_map_1280x720_60_8bit_rgb.ppm ) do (

echo # %%I

REM start with this images
 if "%%I" == ".\Testimages\PPM_trimmed\feature-screen-rea_760x712.ppm" (
 	set flag=1
  echo !flag!
 )

echo %%I
 if !flag! == 1 (


echo # %%I >> %res%



echo Start Encoder: !time:~0,8!
echo Start Encoder: !time:~0,8! >> %res%


   %ENCODER% -i %%I  -o %Outfile%  -method SCF -HXC2auto 1 -tilewidth 9999 -tileheight 9999 -histmode comp -log_name %Logfile% >> %res% 

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