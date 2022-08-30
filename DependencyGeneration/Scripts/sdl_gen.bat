@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat


ECHO.
ECHO Sync SDL repo

SET SDL_DIR=../../../SDL
IF NOT EXIST %SDL_DIR% EXIT /B 1
CD "%SDL_DIR%"

ECHO Checkout a specific version (release-2.24.0, 2022/08/19)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 8c9beb0c873f6ca5efbd88f1ad2648bfc793b2ac


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


ECHO.
ECHO Build projects

CALL %MSBUILD% "VisualC/SDL/SDL.vcxproj" /property:Configuration=Debug /property:Platform=x64
CALL %MSBUILD% "VisualC/SDL/SDL.vcxproj" /property:Configuration=Release /property:Platform=x64
CALL %MSBUILD% "VisualC/SDLmain/SDLmain.vcxproj" /property:Configuration=Debug /property:Platform=x64
CALL %MSBUILD% "VisualC/SDLmain/SDLmain.vcxproj" /property:Configuration=Release /property:Platform=x64


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


EXIT /B 0
