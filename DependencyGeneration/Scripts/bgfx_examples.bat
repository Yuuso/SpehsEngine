@ECHO OFF

SET BGFX_DIR=../../bgfx
IF NOT EXIST %BGFX_DIR% EXIT /B 1
CD "%BGFX_DIR%"

RMDIR .build /S /Q
SET GENIE="../bx/tools/bin/windows/genie.exe"
CALL %GENIE% --with-tools --with-dynamic-runtime --with-examples --with-windows=10.0 vs2019
