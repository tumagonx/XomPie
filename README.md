# XomPie
XomPie is pun of XP and Zombie

This project doesn't intent to be Alky which require system file transplant from newer Windows nor this involve complex patching of target binaries. You'll need Visual C Runtime 2015 and 2013 redist installed for XP.

This is a simple wrapper (at the moment, just kernel32.dll) that might allow you to run applications that "artificially" drop XP support.

When Windows run executable, PE loader will check the kind of EXE (is it DOS, Win32, Win64 etc) including subsysver value in the header. When everything OK EXE will launched and start checkin its direct dependencies.

- Subsysver issue happen when it set above XP version: 5.1 and can be fixed using open source software called pehdr-lite which can patch the value in the executable. Usually you only do this on *.exe file.
It would nice though if there is MITM method that bypass the check...

Sometime for example executable made by MinGW-W64, subsysver doesn't match with _WIN32_WINNT value, in this case this step isn't necessary.

- Dependency issue can be very complex (please use Dependency Walker to analyse your target). This kernel wrapper behave like api-ms-win.dll does by forwarding existing functions into kernel32.dll and k32* into psapi.dll with additional function from msvcp140.dll and few internal functions from WINE project (see below). In short: very few compared to the total missing API.

When something missing (red) from kernel32.dll you need to rename "kernel32.dll" into "kernelxp.dll" in the dll or exe IAT. One of popular application to do this is PE Explorer from NTCore, though you can do this with sed as well with extra cautions. It is also possible to use manifest to load wrapper (without patching IAT) but since we still need to patch the exe (for subsysver) it's pointless and the method may trigger DEP as well. 

Once both solved give your target executable a try.

Example of "running" apps:
Python 3.5,
Adobe DNG Converter 9.6,
MS Visual True Type

Which APIs exported can be seen in the .def files
