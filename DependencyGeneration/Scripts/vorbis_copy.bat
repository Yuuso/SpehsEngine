@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat

SET VORBIS_DIR=../../../vorbis
SET OPTIONS=/NJH /NJS /NDL /NP /NS /NC


ECHO.
ECHO Copy to dependencies

IF EXIST "%DEPS_DIR%/include/vorbis" RMDIR "%DEPS_DIR%/include/vorbis" /S /Q
MKDIR "%DEPS_DIR%/include/vorbis"
ROBOCOPY "%VORBIS_DIR%/include/vorbis"              "%DEPS_DIR%/include/vorbis"             *.h*                /S %OPTIONS%

MKDIR "%DEPS_DIR%/lib/Win32/Debug"
MKDIR "%DEPS_DIR%/lib/Win32/Release"
MKDIR "%DEPS_DIR%/lib/x64/Debug"
MKDIR "%DEPS_DIR%/lib/x64/Release"
ROBOCOPY "%VORBIS_DIR%/win32/VS2010/Win32/Debug"    "%DEPS_DIR%/lib/Win32/Debug"            libvorbis.lib       %OPTIONS%
ROBOCOPY "%VORBIS_DIR%/win32/VS2010/Win32/Debug"    "%DEPS_DIR%/lib/Win32/Debug"            libvorbisfile.lib   %OPTIONS%
ROBOCOPY "%VORBIS_DIR%/win32/VS2010/Win32/Release"  "%DEPS_DIR%/lib/Win32/Release"          libvorbis.lib       %OPTIONS%
ROBOCOPY "%VORBIS_DIR%/win32/VS2010/Win32/Release"  "%DEPS_DIR%/lib/Win32/Release"          libvorbisfile.lib   %OPTIONS%
ROBOCOPY "%VORBIS_DIR%/win32/VS2010/x64/Debug"      "%DEPS_DIR%/lib/x64/Debug"              libvorbis.lib       %OPTIONS%
ROBOCOPY "%VORBIS_DIR%/win32/VS2010/x64/Debug"      "%DEPS_DIR%/lib/x64/Debug"              libvorbisfile.lib   %OPTIONS%
ROBOCOPY "%VORBIS_DIR%/win32/VS2010/x64/Release"    "%DEPS_DIR%/lib/x64/Release"            libvorbis.lib       %OPTIONS%
ROBOCOPY "%VORBIS_DIR%/win32/VS2010/x64/Release"    "%DEPS_DIR%/lib/x64/Release"            libvorbisfile.lib   %OPTIONS%


EXIT /B 0
