@echo off
:queue
for %%I in ("upx.exe") do if exist "%%~$PATH:I" if exist "%~1" upx -d "%~dpnx1"
shift /1
if not "%1" EQU "" goto :queue
pause
