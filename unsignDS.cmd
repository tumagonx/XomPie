@echo off
:queue
if exist "%~1" attrib -r "%~1"
for %%I in ("osslsigncode.exe") do if exist "%%~$PATH:I" osslsigncode remove-signature -in "%~dpnx1" -out "%~dpn1_%~x1"
shift /1
if not "%1" EQU "" goto :queue
pause
