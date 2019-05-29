@echo off 
REM damit Zeit aktuakisiert wird:
SetLocal EnableDelayedExpansion

set ENCODER="..\bin\TSIPcoder_2_12j.exe"
set DECODER="..\bin\TSIPcoder_2_12j.exe"
set PSNR="PSNR_C.exe"

set DIR=C:\Users\Strutz\Documents\Texte\Paper\2016-IEEE-JETCAS-SCF\Investigations\Testimages\PPM_trimmed

set res="01_SCF_2_12k2.txt"
set Outfile="tmp212k2.tsip"
set Logfile="log_SCF_2_12k2_auto.txt"
set Recofile="reco212k2.ppm"

set flag=1
REM 
for  %%I in (%DIR%\*.ppm) do (

REM   for  %%I in (%DIR%\global-multi-scree_560x688.ppm   ) do (
REM   for  %%I in (%DIR%\feed_content_bb_616x456.ppm  ) do (

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

REM    %DECODER%  -i %Outfile% -o %Recofile%   >> %res% 

echo Finished Decoder !time:~0,8!
echo Finished Decoder !time:~0,8! >> %res%

REM  %PSNR% -i1  %%I -i2 %Recofile%  -p  >> %res%

REM     del %Recofile%

   )
  )
  echo    #  %%I >> %res%

)

REM #############  SET 2 ###################
set DIR=C:\Users\Strutz\Documents\Texte\Paper\2016-IEEE-JETCAS-SCF\Investigations\Testimages\SCC_Testsequence\PPM\RGB

set flag=1
REM 
for  %%I in (%DIR%\*.ppm) do (

echo # %%I

REM start with this images
 if "%%I" == "%DIR%\sc_programming_1280x720_60_8bit_rgb.ppm" (
 	set flag=1
  echo !flag!
 )
 
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