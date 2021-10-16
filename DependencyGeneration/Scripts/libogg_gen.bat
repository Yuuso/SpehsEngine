@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat


ECHO.
ECHO Sync libogg repo

SET OGG_DIR=../../../libogg
IF NOT EXIST %OGG_DIR% EXIT /B 1
CD "%OGG_DIR%"

ECHO Checkout a specific version (master branch, 2013/05/14)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout ab78196fd59ad7a329a2b19d2bcec5d840a9a21f


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


ECHO.
ECHO Build projects

CALL %MSBUILD% "win32/VS2010/libogg_static.sln" /property:Configuration=Debug   /property:Platform=Win32    /property:PlatformToolset=v142
CALL %MSBUILD% "win32/VS2010/libogg_static.sln" /property:Configuration=Release /property:Platform=Win32    /property:PlatformToolset=v142
CALL %MSBUILD% "win32/VS2010/libogg_static.sln" /property:Configuration=Debug   /property:Platform=x64      /property:PlatformToolset=v142
CALL %MSBUILD% "win32/VS2010/libogg_static.sln" /property:Configuration=Release /property:Platform=x64      /property:PlatformToolset=v142


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


REN "win32\VS2010\Win32\Debug\libogg_static.lib"    "libogg.lib"
REN "win32\VS2010\Win32\Release\libogg_static.lib"  "libogg.lib"
REN "win32\VS2010\x64\Debug\libogg_static.lib"      "libogg.lib"
REN "win32\VS2010\x64\Release\libogg_static.lib"    "libogg.lib"


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


EXIT /B 0
