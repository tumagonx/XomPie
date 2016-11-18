@echo off
echo STOP: ONLY CONTINUE IF YOU KNOW WHAT YOU DO AND HAVE TESTED THE INSTALLER ON VM!
echo Enter to continue...
pause>nul 2>&1
:queue
for %%I in ("msiexec7.exe") do if exist "%%~$PATH:I" if exist "%~1" if /i "%~x1" equ ".msi" msiexec7 /i "%~dpnx1"
shift /1
if not "%1" EQU "" goto :queue
pause
