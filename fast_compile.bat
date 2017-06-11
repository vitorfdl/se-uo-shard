@ECHO OFF

REM -- If a special path is needed to ecompile.exe set it here
REM -- Path is considered to be run from the root if started by starthere.bat
SET ECOMPILE_PATH=.\tcompile.exe
REM ----------


%ECOMPILE_PATH% -b -r -u

:QUIT()
