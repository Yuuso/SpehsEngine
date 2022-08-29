@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat


ECHO.
ECHO Sync BX repo

SET BX_DIR=../../../bx
IF NOT EXIST %BX_DIR% EXIT /B 1
CD "%BX_DIR%"

ECHO Checkout a specific version (master branch, 2022/08/29)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 44b737b5ea6eef82414d6d94a7e373c2953b85c7


ECHO.
ECHO Sync BIMG repo

SET BIMG_DIR=../bimg
IF NOT EXIST %BIMG_DIR% EXIT /B 1
CD "%BIMG_DIR%"

ECHO Checkout a specific version (master branch, 2022/08/26)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 90ac47dfb0ac21e602ed46a91fcc34016e565b98


ECHO.
ECHO Sync BGFX repo

SET BGFX_DIR=../bgfx
IF NOT EXIST %BGFX_DIR% EXIT /B 1
CD "%BGFX_DIR%"

ECHO Checkout a specific version (master branch, 2022/08/29)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 4613bd1e02c82ea568dfa4bd56623fed82229654


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
