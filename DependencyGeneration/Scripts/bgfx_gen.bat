@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat


ECHO.
ECHO Sync BX repo

SET BX_DIR=../../../bx
IF NOT EXIST %BX_DIR% EXIT /B 1
CD "%BX_DIR%"

ECHO Checkout a specific version (master branch, 2021/09/06)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout e50536ac03eb404f50123d23f2c9ab2b3e4663e7


ECHO.
ECHO Sync BIMG repo

SET BIMG_DIR=../bimg
IF NOT EXIST %BIMG_DIR% EXIT /B 1
CD "%BIMG_DIR%"

ECHO Checkout a specific version (master branch, 2021/09/06)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 6693de0e50ff7e76a22d6f37251fa2dec12168cd


ECHO.
ECHO Sync BGFX repo

SET BGFX_DIR=../bgfx
IF NOT EXIST %BGFX_DIR% EXIT /B 1
CD "%BGFX_DIR%"

ECHO Checkout a specific version (master branch, 2021/09/15)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 71a87b653cd4b5671104fe49e2e38cf5dd4d8675


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


ECHO.
ECHO Generate projects

SET BX_DIR=
SET BIMG_DIR=
SET GENIE="../bx/tools/bin/windows/genie.exe"
CALL %GENIE% --with-tools --with-dynamic-runtime --with-windows=10.0 vs2019


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


ECHO.
ECHO Build projects

CALL %MSBUILD% ".build/projects/vs2019/bgfx.sln" /property:Configuration=Debug /property:Platform=Win32
CALL %MSBUILD% ".build/projects/vs2019/bgfx.sln" /property:Configuration=Release /property:Platform=Win32
CALL %MSBUILD% ".build/projects/vs2019/bgfx.sln" /property:Configuration=Debug /property:Platform=x64
CALL %MSBUILD% ".build/projects/vs2019/bgfx.sln" /property:Configuration=Release /property:Platform=x64


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


EXIT /B 0
