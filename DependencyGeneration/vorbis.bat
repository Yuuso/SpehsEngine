@ECHO OFF

CALL Scripts/vorbis_gen.bat
IF %ERRORLEVEL% NEQ 0 GOTO :FAILED
CALL Scripts/vorbis_copy.bat
IF %ERRORLEVEL% NEQ 0 GOTO :FAILED

PAUSE
EXIT /B 0

:FAILED
ECHO Error!
PAUSE