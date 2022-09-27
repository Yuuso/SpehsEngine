@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat


ECHO.
ECHO Sync BX repo

SET BX_DIR=../../../bx
IF NOT EXIST %BX_DIR% EXIT /B 1
CD "%BX_DIR%"

ECHO Checkout a specific version (master branch, 2022/09/13)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 2d57ed63c0e0c733059f8e437cb6f02779cf4f1a


ECHO.
ECHO Sync BIMG repo

SET BIMG_DIR=../bimg
IF NOT EXIST %BIMG_DIR% EXIT /B 1
CD "%BIMG_DIR%"

ECHO Checkout a specific version (master branch, 2022/09/24)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 0de8816a8b155fe85583aa74f5bc93bdfb8910bb


ECHO.
ECHO Sync BGFX repo

SET BGFX_DIR=../bgfx
IF NOT EXIST %BGFX_DIR% EXIT /B 1
CD "%BGFX_DIR%"

ECHO Checkout a specific version (master branch, 2022/09/14)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 11ac9f27c7c7c38d078824511f223946a43bc3dc


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

CALL %MSBUILD% ".build/projects/vs2019/bgfx.sln" /property:Configuration=Debug /property:Platform=x64
CALL %MSBUILD% ".build/projects/vs2019/bgfx.sln" /property:Configuration=Release /property:Platform=x64


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


EXIT /B 0
