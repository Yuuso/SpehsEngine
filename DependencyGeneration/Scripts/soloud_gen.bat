@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat


ECHO.
ECHO Sync SoLoud repo

SET SOLOUD_DIR=../../../soloud
IF NOT EXIST %SOLOUD_DIR% EXIT /B 1
CD "%SOLOUD_DIR%"

ECHO Checkout a specific version (RELEASE_20200207, 2020/07/02)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout c8e339fdce5c7107bdb3e64bbf707c8fd3449beb


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


ECHO.
ECHO Generate projects

CD build

REM TODO: Use actual GENie repository version
SET GENIE="../../bx/tools/bin/windows/genie.exe"
CALL %GENIE% --with-miniaudio-only --platform=x64 vs2019


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


ECHO.
ECHO Build projects

CALL %MSBUILD% "vs2019/SoloudStatic.vcxproj" /property:Configuration=Debug /property:Platform=x64
CALL %MSBUILD% "vs2019/SoloudStatic.vcxproj" /property:Configuration=Release /property:Platform=x64


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


EXIT /B 0
