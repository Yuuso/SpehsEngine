@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat

SET OGG_DIR=../../../libogg
SET OPTIONS=/NJH /NJS /NDL /NP /NS /NC


ECHO.
ECHO Copy to dependencies

IF EXIST "%DEPS_DIR%/include/ogg" RMDIR "%DEPS_DIR%/include/ogg" /S /Q
MKDIR "%DEPS_DIR%/include/ogg"
ROBOCOPY "%OGG_DIR%/include/ogg"                "%DEPS_DIR%/include/ogg"               *.h*                         /S %OPTIONS%

MKDIR "%DEPS_DIR%/lib/Win32/Debug"
MKDIR "%DEPS_DIR%/lib/Win32/Release"
MKDIR "%DEPS_DIR%/lib/x64/Debug"
MKDIR "%DEPS_DIR%/lib/x64/Release"
ROBOCOPY "%OGG_DIR%/win32/VS2010/Win32/Debug"   "%DEPS_DIR%/lib/Win32/Debug"            libogg.lib           %OPTIONS%
ROBOCOPY "%OGG_DIR%/win32/VS2010/Win32/Release" "%DEPS_DIR%/lib/Win32/Release"          libogg.lib           %OPTIONS%
ROBOCOPY "%OGG_DIR%/win32/VS2010/x64/Debug"     "%DEPS_DIR%/lib/x64/Debug"              libogg.lib           %OPTIONS%
ROBOCOPY "%OGG_DIR%/win32/VS2010/x64/Release"   "%DEPS_DIR%/lib/x64/Release"            libogg.lib           %OPTIONS%


EXIT /B 0
