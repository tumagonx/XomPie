@echo off
isadmin>nul
if "%ERRORLEVEL%" equ "1" echo WARNING: you have no administrative privileges.
for %%I in ("gsar.exe") do if not exist "%%~$PATH:I" echo Error: gsar not found&pause & goto :EOF
for %%I in ("pehdr-lite.exe") do if not exist "%%~$PATH:I" echo Error: pehdr-lite not found&pause & goto :EOF
if not exist "%~1\*" echo Error: not a directory&pause & goto :EOF
cd /d "%~1"
if "%ERRORLEVEL%" equ "1" echo Error: directory not accessible&pause & goto :EOF
echo Will perform patch to all executables in: "%~1" 
echo Enter to continue...
pause>nul 2>&1
for /f "delims=/ usebackq" %%F in (`dir *.exe *.dll /s /a-d /b /oen`) do (
echo.
echo Patching "%%~nxF"...
attrib -r "%%~F" && copy /y "%%~F" "%%~dpnxF.bak" >nul 2>&1
gsar -o -i -s:x00kernel32.dll:x00 -r:x00kernelxp.dll:x00 "%%~dpnxF"
gsar -o -i -s:x00advapi32.dll:x00 -r:x00advapixp.dll:x00 "%%~dpnxF"
pehdr-lite "%%~dpnxF" -subsysver 5.1
)
echo Finished...
pause