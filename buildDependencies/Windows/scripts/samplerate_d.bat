@ECHO OFF

SET LOC_PATH=%CD%
SET FILES=%LOC_PATH%\samplerate_d.txt
SET OUTDIR="%LIBXCONVOLVER_PATH%\Libs\samplerate"

CALL dlextract.bat SAMPLERATE %FILES%

IF EXIST %OUTDIR% rmdir %OUTDIR% /S /Q

mkdir %OUTDIR%

cd %TMP_PATH%

;xcopy libsamplerate-0.1.8 "%OUTDIR%\src" /E /Q /I /Y

:: delete all temporary files
rmdir libsamplerate-0.1.8 /S /Q
del *.* /S /Q

cd %LOC_PATH%
