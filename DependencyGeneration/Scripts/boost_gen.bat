@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat


ECHO.
ECHO Sync boost repo

SET BOOST_DIR=../../../boost
IF NOT EXIST %BOOST_DIR% EXIT /B 1
CD "%BOOST_DIR%"

ECHO Checkout a specific version (1.77.0)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 9d3f9bcd7d416880d4631d7d39cceeb4e8f25da0


EXIT /B 0
