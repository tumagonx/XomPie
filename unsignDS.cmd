@echo off
for %%I in ("osslsigncode.exe") do if exist "%~1" osslsigncode remove-signature -in "%~dpnx1" -out "%~dpnx1"
pause
