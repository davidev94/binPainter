@ECHO OFF

CD ..

SETLOCAL

SET filename=binPainter

REM SET PATH=C:\cygwin64\bin
REM x86_64-w64-mingw32-gcc src\%filename%.c -o build\%filename%.exe

REM SET PATH=C:\Program Files (x86)\Dev-Cpp\MinGW64\bin
SET PATH=C:\Program Files (x86)\Embarcadero\Dev-Cpp\TDM-GCC-64\bin
gcc.exe src\%filename%.c -o build\%filename%.exe

ENDLOCAL
