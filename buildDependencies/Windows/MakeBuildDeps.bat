@ECHO OFF

SET CUR_PATH=%CD%
SET LIBXCONVOLVER_PATH=%CD%\..\..

CD %CUR_PATH%\scripts

ECHO set Visual C++ build environment
CALL "%VS120COMNTOOLS%..\..\VC\bin\vcvars32.bat"

FOR /F "tokens=*" %%S IN ('dir /B "*_b.bat"') DO (
  echo running %%S ...
  CALL %%S
)

CD %CUR_PATH%