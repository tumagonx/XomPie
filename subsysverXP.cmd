@echo off
for %%I in ("pehdr-lite.exe") do if exist "%%~$PATH:I" if exist "%~1" pehdr-lite "%~dpnx1" -subsysver 5.1
pause
