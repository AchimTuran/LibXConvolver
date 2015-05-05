@ECHO OFF

SET LOC_PATH=%CD%
SET FILES=%LOC_PATH%\KissFFT_d.txt
SET OUTDIR="%LIBXCONVOLVER_PATH%\Libs\KissFFT\"

CALL dlextract.bat KissFFT %FILES%

IF EXIST %OUTDIR% rmdir %OUTDIR% /S /Q

mkdir %OUTDIR%

cd %TMP_PATH%

copy CHANGELOG "%OUTDIR%"
copy COPYING "%OUTDIR%"
copy README "%OUTDIR%"
copy README.simd "%OUTDIR%"
copy TIPS "%OUTDIR%"
copy kiss_fft.h "%OUTDIR%"
copy kiss_fft.hh "%OUTDIR%"
copy kiss_fft.c "%OUTDIR%"

:: delete all temporary files
del *.* /S /Q

cd %LOC_PATH%
