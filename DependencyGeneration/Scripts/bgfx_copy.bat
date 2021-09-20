@ECHO OFF
SETLOCAL
CD /D %~dp0

CALL deps_vars.bat

SET BGFX_DIR=../../../bgfx
SET OPTIONS=/NJH /NJS /NDL /NP /NS /NC


ECHO.
ECHO Copy to dependencies

IF EXIST "%DEPS_DIR%/include/bgfx" RMDIR "%DEPS_DIR%/include/bgfx" /S /Q
MKDIR "%DEPS_DIR%/include/bgfx"
ROBOCOPY "%BGFX_DIR%/include/bgfx"              "%DEPS_DIR%/include/bgfx"               *.*                         /S %OPTIONS%

IF EXIST "%DEPS_DIR%/tools/bgfx" RMDIR "%DEPS_DIR%/tools/bgfx" /S /Q
MKDIR "%DEPS_DIR%/tools/bgfx"
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/tools/bgfx"                 *.exe                       %OPTIONS%

MKDIR "%DEPS_DIR%/lib/Win32/Debug"
MKDIR "%DEPS_DIR%/lib/Win32/Release"
MKDIR "%DEPS_DIR%/lib/x64/Debug"
MKDIR "%DEPS_DIR%/lib/x64/Release"
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/lib/Win32/Debug"            bgfxDebug.lib               %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/lib/Win32/Debug"            bimg_decodeDebug.lib        %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/lib/Win32/Debug"            bimg_encodeDebug.lib        %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/lib/Win32/Debug"            bimgDebug.lib               %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/lib/Win32/Debug"            bxDebug.lib                 %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/lib/Win32/Release"          bgfxRelease.lib             %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/lib/Win32/Release"          bimg_decodeRelease.lib      %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/lib/Win32/Release"          bimg_encodeRelease.lib      %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/lib/Win32/Release"          bimgRelease.lib             %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/lib/Win32/Release"          bxRelease.lib               %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/lib/x64/Debug"              bgfxDebug.lib               %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/lib/x64/Debug"              bimg_decodeDebug.lib        %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/lib/x64/Debug"              bimg_encodeDebug.lib        %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/lib/x64/Debug"              bimgDebug.lib               %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/lib/x64/Debug"              bxDebug.lib                 %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/lib/x64/Release"            bgfxRelease.lib             %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/lib/x64/Release"            bimg_decodeRelease.lib      %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/lib/x64/Release"            bimg_encodeRelease.lib      %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/lib/x64/Release"            bimgRelease.lib             %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/lib/x64/Release"            bxRelease.lib               %OPTIONS%

IF EXIST "%DEPS_DIR%/debug/bgfx" RMDIR "%DEPS_DIR%/debug/bgfx" /S /Q
MKDIR "%DEPS_DIR%/debug/bgfx/Win32/Debug"
MKDIR "%DEPS_DIR%/debug/bgfx/Win32/Release"
MKDIR "%DEPS_DIR%/debug/bgfx/x64/Debug"
MKDIR "%DEPS_DIR%/debug/bgfx/x64/Release"
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/Win32/Debug"     bgfxDebug.pdb               %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/Win32/Debug"     bimg_decodeDebug.pdb        %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/Win32/Debug"     bimg_encodeDebug.pdb        %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/Win32/Debug"     bimgDebug.pdb               %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/Win32/Debug"     bxDebug.pdb                 %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/Win32/Release"   bgfxRelease.pdb             %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/Win32/Release"   bimg_decodeRelease.pdb      %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/Win32/Release"   bimg_encodeRelease.pdb      %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/Win32/Release"   bimgRelease.pdb             %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win32_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/Win32/Release"   bxRelease.pdb               %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/x64/Debug"       bgfxDebug.pdb               %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/x64/Debug"       bimg_decodeDebug.pdb        %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/x64/Debug"       bimg_encodeDebug.pdb        %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/x64/Debug"       bimgDebug.pdb               %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/x64/Debug"       bxDebug.pdb                 %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/x64/Release"     bgfxRelease.pdb             %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/x64/Release"     bimg_decodeRelease.pdb      %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/x64/Release"     bimg_encodeRelease.pdb      %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/x64/Release"     bimgRelease.pdb             %OPTIONS%
ROBOCOPY "%BGFX_DIR%/.build/win64_vs2019/bin"   "%DEPS_DIR%/debug/bgfx/x64/Release"     bxRelease.pdb               %OPTIONS%


EXIT /B 0
