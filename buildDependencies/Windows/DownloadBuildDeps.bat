@ECHO OFF

SETLOCAL

SET CUR_PATH=%CD%
SET LIBXCONVOLVER_PATH=%CD%\..\..
ECHO %LIBXCONVOLVER_PATH%
SET TMP_PATH=%CD%\scripts\tmp

ECHO TMP_PATH: %TMP_PATH%
rem can't run rmdir and md back to back. access denied error otherwise.
IF EXIST %LIBXCONVOLVER_PATH%\Libs rmdir %LIBXCONVOLVER_PATH%\Libs /S /Q
IF EXIST %TMP_PATH% rmdir %TMP_PATH% /S /Q

IF $%1$ == $$ (
  SET DL_PATH="%CD%\downloads"
) ELSE (
  SET DL_PATH="%1"
)

SET WGET=%CUR_PATH%\Tools\wget
SET ZIP=%CUR_PATH%\Tools\7z\7za

IF NOT EXIST %DL_PATH% md %DL_PATH%

::md lib
::md include
md %TMP_PATH%

cd scripts

FOR /F "tokens=*" %%S IN ('dir /B "*_d.bat"') DO (
  echo running %%S ...
  CALL %%S
)

::SET FORMED_OK_FLAG=%TMP_PATH%\got-all-formed-packages
REM Trick to preserve console title
REM start /b /wait cmd.exe /c get_formed.cmd
REM IF NOT EXIST %FORMED_OK_FLAG% (
REM  ECHO ERROR: Not all formed packages are ready!
REM  EXIT /B 101
REM )

cd %CUR_PATH%

::rmdir %TMP_PATH% /S /Q
EXIT /B 0
