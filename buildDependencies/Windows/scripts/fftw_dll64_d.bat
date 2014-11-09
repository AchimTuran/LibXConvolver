@ECHO OFF

SET LOC_PATH=%CD%
SET FILES=%LOC_PATH%\fftw_dll64_d.txt
SET OUTDIR="%LIBXCONVOLVER_PATH%\Libs\fftw3_dll64\"

CALL dlextract.bat fftw3_dll64 %FILES%

IF EXIST %OUTDIR% rmdir %OUTDIR% /S /Q

mkdir %OUTDIR%

cd %TMP_PATH%

copy fftw3.h "%OUTDIR%"
copy NEWS "%OUTDIR%"
copy README "%OUTDIR%"
copy README-WINDOWS "%OUTDIR%"
copy COPYRIGHT "%OUTDIR%"
copy COPYING "%OUTDIR%"
copy libfftw3f-3.def "%OUTDIR%"
copy libfftw3f-3.dll "%OUTDIR%"

:: delete all temporary files
del *.* /S /Q

cd %LOC_PATH%
