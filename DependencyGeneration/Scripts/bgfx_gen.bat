@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat


ECHO.
ECHO Sync BX repo

SET BX_DIR=../../../bx
IF NOT EXIST %BX_DIR% EXIT /B 1
CD "%BX_DIR%"

CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git pull


ECHO.
ECHO Sync BIMG repo

SET BIMG_DIR=../bimg
IF NOT EXIST %BIMG_DIR% EXIT /B 1
CD "%BIMG_DIR%"

CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git pull


ECHO.
ECHO Sync BGFX repo

SET BGFX_DIR=../bgfx
IF NOT EXIST %BGFX_DIR% EXIT /B 1
CD "%BGFX_DIR%"

CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git pull


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
