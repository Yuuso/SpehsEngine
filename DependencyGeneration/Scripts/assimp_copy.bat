@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat

SET ASSIMP_DIR=../../../assimp
SET OPTIONS=/NJH /NJS /NDL /NP /NS /NC


ECHO.
ECHO Copy to dependencies

IF EXIST "%DEPS_DIR%/include/assimp" RMDIR "%DEPS_DIR%/include/assimp" /S /Q
MKDIR "%DEPS_DIR%/include/assimp"
ROBOCOPY "%ASSIMP_DIR%/include/assimp"          "%DEPS_DIR%/include/assimp"             *.*                         /S %OPTIONS%
ROBOCOPY "%ASSIMP_DIR%/build/x64/include/assimp" "%DEPS_DIR%/include/assimp"            *.*                         /S %OPTIONS%

IF NOT EXIST "%DEPS_DIR%/lib/Win32/Debug" MKDIR "%DEPS_DIR%/lib/Win32/Debug"
IF NOT EXIST "%DEPS_DIR%/lib/Win32/Release" MKDIR "%DEPS_DIR%/lib/Win32/Release"
IF NOT EXIST "%DEPS_DIR%/lib/x64/Debug" MKDIR "%DEPS_DIR%/lib/x64/Debug"
IF NOT EXIST "%DEPS_DIR%/lib/x64/Release" MKDIR "%DEPS_DIR%/lib/x64/Release"
ROBOCOPY "%ASSIMP_DIR%/build/Win32/lib/Debug"   "%DEPS_DIR%/lib/Win32/Debug"            assimp-vc142-mtd.lib        %OPTIONS%
ROBOCOPY "%ASSIMP_DIR%/build/Win32/lib/Release" "%DEPS_DIR%/lib/Win32/Release"          assimp-vc142-mt.lib         %OPTIONS%
ROBOCOPY "%ASSIMP_DIR%/build/x64/lib/Debug"     "%DEPS_DIR%/lib/x64/Debug"              assimp-vc142-mtd.lib        %OPTIONS%
ROBOCOPY "%ASSIMP_DIR%/build/x64/lib/Release"   "%DEPS_DIR%/lib/x64/Release"            assimp-vc142-mt.lib         %OPTIONS%

IF EXIST "%DEPS_DIR%/debug/assimp" RMDIR "%DEPS_DIR%/debug/assimp" /S /Q
MKDIR "%DEPS_DIR%/debug/assimp/Win32"
MKDIR "%DEPS_DIR%/debug/assimp/x64"
ROBOCOPY "%ASSIMP_DIR%/build/Win32/bin/Debug"   "%DEPS_DIR%/debug/assimp/Win32"         assimpd.pdb                 %OPTIONS%
ROBOCOPY "%ASSIMP_DIR%/build/x64/bin/Debug"     "%DEPS_DIR%/debug/assimp/x64"           assimpd.pdb                 %OPTIONS%


EXIT /B 0
