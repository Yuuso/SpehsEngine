@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat


ECHO.
ECHO Sync vorbis repo

SET VORBIS_DIR=../../../vorbis
IF NOT EXIST %VORBIS_DIR% EXIT /B 1
CD "%VORBIS_DIR%"

ECHO Checkout a specific version (v1.3.7)
CALL git reset --hard HEAD
CALL git clean -d -x -f
CALL git checkout 0657aee69dec8508a0011f47f3b69d7538e9d262


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


ECHO.
ECHO Build projects

CALL %MSBUILD% "win32/VS2010/vorbis_static.sln" /property:Configuration=Debug   /property:Platform=Win32    /property:PlatformToolset=v142  /property:IncludePath="../../../libogg/win32/VS2010/Win32/Debug;$(IncludePath)"
CALL %MSBUILD% "win32/VS2010/vorbis_static.sln" /property:Configuration=Release /property:Platform=Win32    /property:PlatformToolset=v142  /property:IncludePath="../../../libogg/win32/VS2010/Win32/Release;$(IncludePath)"
CALL %MSBUILD% "win32/VS2010/vorbis_static.sln" /property:Configuration=Debug   /property:Platform=x64      /property:PlatformToolset=v142  /property:IncludePath="../../../libogg/win32/VS2010/x64/Debug;$(IncludePath)"
CALL %MSBUILD% "win32/VS2010/vorbis_static.sln" /property:Configuration=Release /property:Platform=x64      /property:PlatformToolset=v142  /property:IncludePath="../../../libogg/win32/VS2010/x64/Release;$(IncludePath)"


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


REN "win32\VS2010\Win32\Debug\libvorbis_static.lib"         "libvorbis.lib"
REN "win32\VS2010\Win32\Debug\libvorbisfile_static.lib"     "libvorbisfile.lib"
REN "win32\VS2010\Win32\Release\libvorbis_static.lib"       "libvorbis.lib"
REN "win32\VS2010\Win32\Release\libvorbisfile_static.lib"   "libvorbisfile.lib"
REN "win32\VS2010\x64\Debug\libvorbis_static.lib"           "libvorbis.lib"
REN "win32\VS2010\x64\Debug\libvorbisfile_static.lib"       "libvorbisfile.lib"
REN "win32\VS2010\x64\Release\libvorbis_static.lib"         "libvorbis.lib"
REN "win32\VS2010\x64\Release\libvorbisfile_static.lib"     "libvorbisfile.lib"


IF %ERRORLEVEL% NEQ 0 EXIT /B 1


EXIT /B 0
