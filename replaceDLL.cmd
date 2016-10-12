@echo off
:: this action completely dubious, please backup before proceed
:: at the moment just kernel32.dll
:queue
for %%I in ("gsar.exe") do if exist "%%~$PATH:I" if exist "%~1" (
gsar -o -i -s:x00kernel32.dll:x00 -r:x00kernelxp.dll:x00 "%~dpnx1"
gsar -o -i -s:x00advapi32.dll:x00 -r:x00advapixp.dll:x00 "%~dpnx1"
)
shift /1
if not "%1" EQU "" goto :queue
pause
