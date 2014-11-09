@ECHO OFF

SET LOC_PATH=%CD%
SET FILES=%LOC_PATH%\sndfile_64_d.txt
SET OUTDIR="%LIBXCONVOLVER_PATH%\Libs\sndfile_64"

CALL dlextract.bat sndfile %FILES%

IF EXIST %OUTDIR% rmdir %OUTDIR% /S /Q

mkdir %OUTDIR%

cd %TMP_PATH%
:: xcopy libsndfile-1.0.25 "%OUTDIR%\src" /E /Q /I /Y
xcopy lib %OUTDIR% /E /Q /I /Y
copy NEWS %OUTDIR%
copy ChangeLog %OUTDIR%
copy bin\libsndfile-1.dll "%OUTDIR%\libsndfile-1.dll"
copy include\sndfile.h "%OUTDIR%\sndfile.h"
copy include\sndfile.hh "%OUTDIR%\sndfile.hh"

:: delete all temporary files
rmdir bin /S /Q
rmdir include /S /Q
rmdir lib /S /Q
rmdir Projects /S /Q
rmdir share /S /Q
del *.* /S /Q

cd %LOC_PATH%
