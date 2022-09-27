@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat

SET SDL_DIR=../../../sdl
SET OPTIONS=/NJH /NJS /NDL /NP /NS /NC


ECHO.
ECHO Copy to dependencies

IF EXIST "%DEPS_DIR%/include/SDL" RMDIR "%DEPS_DIR%/include/SDL" /S /Q
MKDIR "%DEPS_DIR%/include/SDL"
ROBOCOPY "%SDL_DIR%/include/"                       "%DEPS_DIR%/include/SDL"                *.h                         /S %OPTIONS%

MKDIR "%DEPS_DIR%/lib/x64/Debug"
MKDIR "%DEPS_DIR%/lib/x64/Release"
ROBOCOPY "%SDL_DIR%/VisualC/SDL/x64/Debug/"         "%DEPS_DIR%/lib/x64/Debug"              SDL2.lib                    %OPTIONS%
ROBOCOPY "%SDL_DIR%/VisualC/SDL/x64/Release/"       "%DEPS_DIR%/lib/x64/Release"            SDL2.lib                    %OPTIONS%
ROBOCOPY "%SDL_DIR%/VisualC/SDLmain/x64/Debug/"     "%DEPS_DIR%/lib/x64/Debug"              SDL2main.lib                %OPTIONS%
ROBOCOPY "%SDL_DIR%/VisualC/SDLmain/x64/Release/"   "%DEPS_DIR%/lib/x64/Release"            SDL2main.lib                %OPTIONS%

MKDIR "%DEPS_DIR%/dll/x64"
ROBOCOPY "%SDL_DIR%/VisualC/SDL/x64/Release/"       "%DEPS_DIR%/dll/x64"                    SDL2.dll                    %OPTIONS%

IF EXIST "%DEPS_DIR%/debug/SDL" RMDIR "%DEPS_DIR%/debug/SDL" /S /Q
MKDIR "%DEPS_DIR%/debug/SDL/x64/Debug"
MKDIR "%DEPS_DIR%/debug/SDL/x64/Release"
ROBOCOPY "%SDL_DIR%/VisualC/SDL/x64/Debug/"         "%DEPS_DIR%/debug/SDL/x64/Debug"        SDL2.pdb                    %OPTIONS%
ROBOCOPY "%SDL_DIR%/VisualC/SDL/x64/Release/"       "%DEPS_DIR%/debug/SDL/x64/Release"      SDL2.pdb                    %OPTIONS%


EXIT /B 0
