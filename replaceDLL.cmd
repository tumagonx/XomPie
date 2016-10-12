@echo off
:: this action completely dubious, please backup before proceed
:: at the moment just kernel32.dll
:queue
for %%I in ("gsar.exe") do if exist "%%~$PATH:I" if exist "%~1" (
gsar -o -i -skernel32.dll -rkernelxp.dll "%~dpnx1"
gsar -o -i -sadvapi32.dll -radvapixp.dll "%~dpnx1"
)
shift /1
if not "%1" EQU "" goto :queue
pause
