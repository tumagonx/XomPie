:: supposedly only PE >= 6.0 that should patched, but this not true for MinGW PE which set to 4.0 despite using Vista API
@echo off
for %%I in ("gsar.exe") do if not exist "%%~$PATH:I" echo Error: gsar not found&pause & goto :EOF
for %%I in ("pehdr-lite.exe") do if not exist "%%~$PATH:I" echo Error: pehdr-lite not found&pause & goto :EOF
if "%~1" equ "" echo Error: no directory specified&pause & goto :EOF
if not exist "%~1\*" echo Error: not a directory&pause & goto :EOF
cd /d "%~1"
if not "%ERRORLEVEL%" equ "0" echo Error: directory not accessible&pause & goto :EOF
echo Will perform patch to all executables in: "%~1" 
echo Enter to continue...
pause>nul 2>&1
for /f "delims=/ usebackq" %%F in (`dir *.exe *.dll /s /a-d /b /oen`) do (
echo.
echo Patching "%%~nxF"...
attrib -r "%%~F" && copy /y "%%~F" "%%~dpnxF.bak" >nul 2>&1
gsar -o -i -s:x00kernel32.dll:x00 -r:x00kernelxp.dll:x00 "%%~dpnxF"
gsar -o -i -s:x90kernel32.dll:x00 -r:x90kernelxp.dll:x00 "%%~dpnxF"
gsar -o -i -s:x00k:x00e:x00r:x00n:x00e:x00l:x003:x002:x00.:x00d:x00l:x00l:x00 -r:x00k:x00e:x00r:x00n:x00e:x00l:x00x:x00p:x00.:x00d:x00l:x00l:x00 "%%~dpnxF"
gsar -o -i -s:x00advapi32.dll:x00 -r:x00advapixp.dll:x00 "%%~dpnxF"
gsar -o -i -s:x90advapi32.dll:x00 -r:x90advapixp.dll:x00 "%%~dpnxF"
gsar -o -i -s:x00a:x00d:x00v:x00a:x00p:x00i:x003:x002:x00.:x00d:x00l:x00l:x00 -r:x00a:x00d:x00v:x00a:x00p:x00i:x00x:x00p:x00.:x00d:x00l:x00l:x00 "%%~dpnxF"
pehdr-lite "%%~dpnxF" -subsysver 5.1
)
echo Finished...
pause