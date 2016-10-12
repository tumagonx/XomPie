@echo off
for %%I in ("osslsigncode.exe") do if not exist "%%~$PATH:I" echo Error: osslsigncode not found& goto :EOF
for %%I in ("gsar.exe") do if not exist "%%~$PATH:I" echo Error: gsar not found& goto :EOF
for %%I in ("pehdr-lite.exe") do if not exist "%%~$PATH:I" echo Error: pehdr-lite not found& goto :EOF
if not exist "%~1\*" echo Error: not a directory& goto :EOF
cd /d "%~1"
if "%ERRORLEVEL%" equ "1" echo Error: directory not accessible& goto :EOF
echo Will perform patch to all executables in: "%~1" 
echo Enter to continue...
pause>nul 2>&1
for /f "delims=/ usebackq" %%F in (`dir *.exe *.dll /s /a-d /b /oen`) do (
echo.
echo Patching "%%~nxF"...
attrib -r "%%~F" && copy /y "%%~F" "%%~dpnxF.bak" >nul 2>&1
osslsigncode remove-signature -in "%%~dpnxF.bak" -out "%%~dpnxF"
gsar -o -i -skernel32.dll -rkernelxp.dll "%%~dpnxF"
gsar -o -i -sadvapi32.dll -radvapixp.dll "%%~dpnxF"
pehdr-lite "%%~dpnxF" -subsysver 5.1
)
pause