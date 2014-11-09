@ECHO OFF

SET LOC_PATH=%CD%
SET OUTDIR="%LIBXCONVOLVER_PATH%\Libs\samplerate"

copy samplerate_CMakeLists.txt %OUTDIR%\src\CMakeLists.txt

CD %OUTDIR%\src

copy /y Win32\config.h src\config.h
copy /y Win32\unistd.h examples\unistd.h

CD %OUTDIR%

cmake -G "NMake Makefiles" src
nmake

copy /y src\src\common.h common.h
copy /y src\src\config.h config.h
copy /y src\src\float_cast.h float_cast.h
copy /y src\src\samplerate.h samplerate.h

CD %LOC_PATH%
