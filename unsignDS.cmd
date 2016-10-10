@echo off
:queue
for %%I in ("osslsigncode.exe") do if exist "%%~$PATH:I" if exist "%~1" osslsigncode remove-signature -in "%~dpnx1" -out "%~dpnx1"
shift /1
if not "%1" EQU "" goto :queue
pause
