@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat

SET SOLOUD_DIR=../../../soloud
SET OPTIONS=/NJH /NJS /NDL /NP /NS /NC


ECHO.
ECHO Copy to dependencies

IF EXIST "%DEPS_DIR%/include/soloud" RMDIR "%DEPS_DIR%/include/soloud" /S /Q
MKDIR "%DEPS_DIR%/include/soloud"
ROBOCOPY "%SOLOUD_DIR%/include/"                "%DEPS_DIR%/include/soloud"             *.*                         /S %OPTIONS%

MKDIR "%DEPS_DIR%/lib/x64/Debug"
MKDIR "%DEPS_DIR%/lib/x64/Release"
ROBOCOPY "%SOLOUD_DIR%/lib/"                    "%DEPS_DIR%/lib/x64/Debug"              soloud_static_x64_d.lib     %OPTIONS%
ROBOCOPY "%SOLOUD_DIR%/lib/"                    "%DEPS_DIR%/lib/x64/Release"            soloud_static_x64.lib       %OPTIONS%

IF EXIST "%DEPS_DIR%/debug/soloud" RMDIR "%DEPS_DIR%/debug/soloud" /S /Q
MKDIR "%DEPS_DIR%/debug/soloud/x64/Debug"
ROBOCOPY "%SOLOUD_DIR%/lib/"                    "%DEPS_DIR%/debug/soloud/x64/Debug"     soloud_static_x64_d.idb     %OPTIONS%
ROBOCOPY "%SOLOUD_DIR%/lib/"                    "%DEPS_DIR%/debug/soloud/x64/Debug"     soloud_static_x64_d.pdb     %OPTIONS%


EXIT /B 0
