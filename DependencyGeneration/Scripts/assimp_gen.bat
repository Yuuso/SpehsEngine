@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat


ECHO.
ECHO Sync repo

SET ASSIMP_DIR=../../../assimp
IF NOT EXIST %ASSIMP_DIR% EXIT /B 1
CD "%ASSIMP_DIR%"

CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git pull


ECHO.
ECHO Generate and build projects

SET SOURCE_DIR=.
SET GENERATOR=Visual Studio 16 2019
SET OPTIONS=-DASSIMP_NO_EXPORT=ON -DASSIMP_WERROR=ON -DASSIMP_ERROR_MAX=ON -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF

SET BUILD_DIR="./build/Win32"
CALL %CMAKE% CMakeLists.txt -G "%GENERATOR%" -S %SOURCE_DIR% -B %BUILD_DIR% %OPTIONS%
CALL %CMAKE% --build %BUILD_DIR% --config debug
CALL %CMAKE% --build %BUILD_DIR% --config release

SET BUILD_DIR="./build/x64"
CALL %CMAKE% CMakeLists.txt -G "%GENERATOR%" -S %SOURCE_DIR% -B %BUILD_DIR% %OPTIONS%
CALL %CMAKE% --build %BUILD_DIR% --config debug
CALL %CMAKE% --build %BUILD_DIR% --config release


EXIT /B 0
