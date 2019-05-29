@echo off

:: When the command processor interprets a batch file, it does it like this:
:: - Read a line of code.
:: - Perform whatever variable substitution is necessary.
:: - Execute it.
:: - Read the next line of code.
:: - etc.
:: 
:: And here comes the catch: When you use parenthesis in your code then everything :: within the parenthesis counts as one single line that gets resolved in one fell:: swoop, regardless of the individual steps. In your original code this means that:: fdate is set to some value but the subsequent line (echo %fdate%) is not a line of:: its own and it therefore does not yet "know" the value of %fdate%.
:: 
:: How to get around it? You need to take two measures:
:: a) Tell the command processor to delay resolution of variables until it executes :: them.
:: b) Surround the variables with exclamation marks if you want them to be subject to :: delayed expansion.
:: 
:: When you observe these two rules then you can do anything you like with your :: variables.


SetLocal EnableDelayedExpansion

set res=unique_lines.txt"
echo #unique> %res%

set flag=ABCDE
set imag=

REM for /F = file parsing
REM for /F "tokens=1-27 delims=" %%a in (log_TSIP2.txt) do (

for /f "usebackq delims=" %%L in (log_TSIP2.txt) do (
set line=%%L

REM echo !line!
call :processToken
)
pause
goto :ende

:processToken

for /F "tokens=19-22" %%a in ("%line%") do (  
  
REM    for /F "tokens=18-21" %%a in ("%line%") do (     
   REM set sline=%%a %%b
	REM echo !sline! >> %res%
   if !flag!==ABCDE (
	REM initialisation of tokens
      set flag=1
      REM initialisation of variables with first line
	REM output current line
	echo !line! >> %res%
       set RCT=%%a
      set method=%%b
      set histMode=%%c
      set pred=%%d
   ) else (
    REM  check whether current line is identical to previous
    REM output line if it is different
    if !RCT! NEQ %%a (
      set RCT=%%a
      set method=%%b
      set histMode=%%c
      set pred=%%d
	REM output current line
	echo !line! >> %res%
     )
     if !method! NEQ %%b (
      set RCT=%%a
      set method=%%b
      set histMode=%%c
      set pred=%%d
	REM output current line
	echo !line! >> %res%
     )
     if !histMode! NEQ %%c (
      set RCT=%%a
      set method=%%b
      set histMode=%%c
      set pred=%%d
	REM output current line
	echo !line! >> %res%
      )
     )
     if !pred! NEQ %%d (
      set RCT=%%a
      set method=%%b
      set histMode=%%c
      set pred=%%d
	REM output current linee
	echo !line! >> %res%
     )
    ) 
   )
  )

goto :eof              
REM ":EOF"  führt die Batch-Datei nach dem Aufruf der Sprungmarke fort


:ende



@echo on