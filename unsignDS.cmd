@echo off
:queue
if exist "%~1" attrib -r "%~1" & copy /y "%~1" "%~dpnx1.bak" >nul 2>&1
for %%I in ("osslsigncode.exe") do if exist "%%~$PATH:I" osslsigncode remove-signature -in "%~dpnx1.bak" -out "%~dpnx1"
shift /1
if not "%1" EQU "" goto :queue
pause
