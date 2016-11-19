:: supposedly only PE >= 6.0 that should patched, but this not true for MinGW PE which set to 4.0 by default despite using Vista API

@echo off
echo WARNING: IT IS ILLEGAL TO PATCH SOFTWARE THAT FORBID BINARY MODIFICATION!
echo Enter to continue...
pause>nul 2>&1

set gdiori=g d i 3 2 . d l l 
set gdimod=g d i x p . d l l 
set uspori=u s p 3 2 . d l l 
set uspmod=u s p x p . d l l 
set usrori=u s e r 3 2 . d l l 
set usrmod=u s e r x p . d l l 
set advori=a d v a p i 3 2 . d l l 
set advmod=a d v a p i x p . d l l 
set shlori=s h e l l 3 2 . d l l 
set shlmod=s h e l l x p . d l l 
set crtori=m s v c r t . d l l 
set crtmod=x p v c r t . d l l 
set dexori=d 3 d 9 . d l l 
set dexmod=d 3 d x . d l l 
set wskori=w s 2 _ 3 2 . d l l 
set wskmod=w s 2 _ x p . d l l 
set krnori=k e r n e l 3 2 . d l l 
set krnmod=k e r n e l x p . d l l 
set UPXPE=
set USIGN=
set USPOK=

for %%I in ("gsar.exe") do if not exist "%%~$PATH:I" echo Error: gsar not found&pause & goto :EOF
for %%I in ("pehdr-lite.exe") do if not exist "%%~$PATH:I" echo Error: pehdr-lite not found&pause & goto :EOF
for %%I in ("osslsigncode.exe") do if exist "%%~$PATH:I" set USIGN=1
for %%I in ("upx.exe") do if exist "%%~$PATH:I" set UPXPE=1
if exist "%CommonProgramFiles%\Microsoft Shared\OFFICE14\usp10.dll" set USPOK=1

if "%~1" equ "" echo Error: no input specified&pause & goto :EOF
if not exist "%~1\*" goto :PERFILE
cd /d "%~1"
if not "%ERRORLEVEL%" equ "0" echo Error: directory not accessible&pause & goto :EOF
echo Will perform patch to all executables in: "%~1" 
echo Enter to continue...
pause>nul 2>&1
for /f "delims=/ usebackq" %%F in (`dir *.exe *.dll /s /a-d /b /oen`) do (
echo.
echo Patching "%%~nxF"...
call :PATCHER "%%~dpnxF"
)
echo Finished...
pause& exit
:PERFILE
if not exist "%~1" shift /1&& goto :PERFILE
echo Patching "%~nx1"...
call :PATCHER "%~dpnx1"
shift /1
if not "%1" EQU "" goto :PERFILE
echo Finished...
pause& exit
:PATCHER
attrib -r "%~1" && copy /y "%~1" "%~dpnx1.bak" >nul 2>&1
if "%USIGN%" equ "1" osslsigncode remove-signature -in "%~dpnx1" -out "%TEMP%\%~nx1" && move /y "%TEMP%\%~nx1" "%~dpnx1" >nul 2>&1
if "%UPXPE%" equ "1" upx -d "%~dpnx1" >nul 2>&1
gsar -o -i -s:x00%krnori: =%:x00 -r:x00%krnmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x90%krnori: =%:x00 -r:x90%krnmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x00%krnori: =:x00%:x00 -r:x00%krnmod: =:x00%:x00 "%~dpnx1"
gsar -o -i -s:x00%advori: =%:x00 -r:x00%advmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x90%advori: =%:x00 -r:x90%advmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x00%advori: =:x00%:x00 -r:x00%advmod: =:x00%:x00 "%~dpnx1"
gsar -o -i -s:x00%crtori: =%:x00 -r:x00%crtmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x90%crtori: =%:x00 -r:x90%crtmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x00%crtori: =:x00%:x00 -r:x00%crtmod: =:x00%:x00 "%~dpnx1"
gsar -o -i -s:x00%gdiori: =%:x00 -r:x00%gdimod: =%:x00 "%~dpnx1"
gsar -o -i -s:x90%gdiori: =%:x00 -r:x90%gdimod: =%:x00 "%~dpnx1"
gsar -o -i -s:x00%gdiori: =:x00%:x00 -r:x00%gdimod: =:x00%:x00 "%~dpnx1"
gsar -o -i -s:x00%shlori: =%:x00 -r:x00%shlmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x90%shlori: =%:x00 -r:x90%shlmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x00%shlori: =:x00%:x00 -r:x00%shlmod: =:x00%:x00 "%~dpnx1"
gsar -o -i -s:x00%usrori: =%:x00 -r:x00%usrmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x90%usrori: =%:x00 -r:x90%usrmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x00%usrori: =:x00%:x00 -r:x00%usrmod: =:x00%:x00 "%~dpnx1"
gsar -o -i -s:x00%wskori: =%:x00 -r:x00%wskmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x90%wskori: =%:x00 -r:x90%wskmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x00%wskori: =:x00%:x00 -r:x00%wskmod: =:x00%:x00 "%~dpnx1"
if "%USPOK%" EQU "1" (
gsar -o -i -s:x00%uspori: =%:x00 -r:x00%uspmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x90%uspori: =%:x00 -r:x90%uspmod: =%:x00 "%~dpnx1"
gsar -o -i -s:x00%uspori: =:x00%:x00 -r:x00%uspmod: =:x00%:x00 "%~dpnx1"
)
pehdr-lite "%~dpnx1" -subsysver 5.1

::TODO make exclusion for common ms vc runtime, usually 6.0 enforced in vc 120 above,
::but it can be as early as vc 80 aka vista sdk