@echo off
:: at the moment this is only system files updater
for %%I in ("dwnl.exe") do if not exist "%%~$PATH:I" echo Error: dwnl not found&pause & goto :EOF
echo UPDATING XOMPIE DLL:
pause
set files=advapixp kernelxp xpvcrt shellxp userxp gdixp dxgi dwrite d3d11 d3d10core d3d10_1 d3d10 d2d1 d3dx winetxc wined3d dwmapi wer wevtapi ws2_32
call :XOMDWNL %files%

echo.
echo Done.
pause& exit

:XOMDWNL
set error=
echo %1.dll:
dwnl https://github.com/tumagonx/XomPie/raw/master/%1.dll %SYSTEMROOT%\system32\%1.dll
if %ERRORLEVEL% EQU 1 (
set error=1
set /p answer=Eeek! something wrong, retry [y/n]? 
)
if "%error%" EQU "1" if /i "%answer%" EQU "y" goto :XOMDWNL
shift /1
if not "%~1" EQU "" goto :XOMDWNL
