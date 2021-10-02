
@ECHO OFF
SETLOCAL
CD /D %~dp0

SET SOURCE_LOC=%~1
SET SHADER_NAME=%~2
SET TARGET_LOC=%~3
IF "%~4" EQU "DEBUG" (
	SET SHADERC="..\Dependencies\tools\bgfx\shadercDebug.exe"
) ELSE (
	SET SHADERC="..\Dependencies\tools\bgfx\shadercRelease.exe"
)


ECHO Vertex shaders
ECHO vs_%SHADER_NAME%.sc

ECHO 	glsl
IF NOT EXIST %TARGET_LOC%\glsl MKDIR %TARGET_LOC%\glsl
CALL %SHADERC% ^
--type vertex ^
--platform windows ^
--profile 300_es ^
--varyingdef %SOURCE_LOC%\varying.def.sc ^
-f %SOURCE_LOC%\vs_%SHADER_NAME%.sc ^
-o %TARGET_LOC%\glsl\vs_%SHADER_NAME%.bin

ECHO 	spirv
IF NOT EXIST %TARGET_LOC%\spirv MKDIR %TARGET_LOC%\spirv
CALL %SHADERC% ^
--type vertex ^
--platform windows ^
--profile spirv ^
--varyingdef %SOURCE_LOC%\varying.def.sc ^
-f %SOURCE_LOC%\vs_%SHADER_NAME%.sc ^
-o %TARGET_LOC%\spirv\vs_%SHADER_NAME%.bin

ECHO 	dx9
IF NOT EXIST %TARGET_LOC%\dx9 MKDIR %TARGET_LOC%\dx9
CALL %SHADERC% ^
--type vertex ^
--platform windows ^
--profile vs_3_0 ^
-O 3 ^
--varyingdef %SOURCE_LOC%\varying.def.sc ^
-f %SOURCE_LOC%\vs_%SHADER_NAME%.sc ^
-o %TARGET_LOC%\dx9\vs_%SHADER_NAME%.bin

ECHO 	dx11
IF NOT EXIST %TARGET_LOC%\dx11 MKDIR %TARGET_LOC%\dx11
CALL %SHADERC% ^
--type vertex ^
--platform windows ^
--profile vs_4_0 ^
-O 3 ^
--varyingdef %SOURCE_LOC%\varying.def.sc ^
-f %SOURCE_LOC%\vs_%SHADER_NAME%.sc ^
-o %TARGET_LOC%\dx11\vs_%SHADER_NAME%.bin

ECHO 	metal
IF NOT EXIST %TARGET_LOC%\metal MKDIR %TARGET_LOC%\metal
CALL %SHADERC% ^
--type vertex ^
--platform ios ^
--profile metal ^
--varyingdef %SOURCE_LOC%\varying.def.sc ^
-f %SOURCE_LOC%\vs_%SHADER_NAME%.sc ^
-o %TARGET_LOC%\metal\vs_%SHADER_NAME%.bin


EXIT /B 0
