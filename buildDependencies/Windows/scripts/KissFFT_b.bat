@ECHO OFF

SET LOC_PATH=%CD%
SET OUTDIR="%LIBXCONVOLVER_PATH%\Libs\KissFFT\"

copy KissFFT_CMakeLists.txt %OUTDIR%\src\CMakeLists.txt

CD %OUTDIR%

REM lib /def:libfftw3f-3.def /MACHINE:X86

CD %LOC_PATH%
