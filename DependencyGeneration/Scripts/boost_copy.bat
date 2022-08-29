@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat

SET BOOST_DIR=../../../boost
SET OPTIONS=/NJH /NJS /NDL /NP /NS /NC


ECHO.
ECHO Copy to dependencies

IF EXIST "%DEPS_DIR%/include/boost" RMDIR "%DEPS_DIR%/include/boost" /S /Q
MKDIR "%DEPS_DIR%/include/boost"

REM Actual primary dependencies are asio, signals2, lexical_cast and format
ROBOCOPY "%BOOST_DIR%/libs/align/include/boost"                 "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/array/include/boost"                 "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/asio/include/boost"                  "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/assert/include/boost"                "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/bind/include/boost"                  "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/config/include/boost"                "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/concept_check/include/boost"         "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/container/include/boost"             "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/container_hash/include/boost"        "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/conversion/include/boost"            "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/core/include/boost"                  "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/date_time/include/boost"             "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/detail/include/boost"                "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/format/include/boost"                "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/function/include/boost"              "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/function_types/include/boost"        "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/fusion/include/boost"                "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/integer/include/boost"               "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/lexical_cast/include/boost"          "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/iterator/include/boost"              "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/io/include/boost"                    "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/mp11/include/boost"                  "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/mpl/include/boost"                   "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/move/include/boost"                  "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/multi_index/include/boost"           "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/numeric/conversion/include/boost"    "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/optional/include/boost"              "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/parameter/include/boost"             "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/predef/include/boost"                "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/preprocessor/include/boost"          "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/range/include/boost"                 "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/regex/include/boost"                 "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/signals2/include/boost"              "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/smart_ptr/include/boost"             "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/static_assert/include/boost"         "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/system/include/boost"                "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/throw_exception/include/boost"       "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/tuple/include/boost"                 "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/type_index/include/boost"            "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/type_traits/include/boost"           "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/typeof/include/boost"                "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/utility/include/boost"               "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/variant/include/boost"               "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%
ROBOCOPY "%BOOST_DIR%/libs/winapi/include/boost"                "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%

REM Not currently used by SpehsEngine!
ROBOCOPY "%BOOST_DIR%/libs/stacktrace/include/boost"            "%DEPS_DIR%/include/boost"       *.*         /S %OPTIONS%

EXIT /B 0
