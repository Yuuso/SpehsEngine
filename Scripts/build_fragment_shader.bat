
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


ECHO Fragment shaders
ECHO fs_%SHADER_NAME%.sc

ECHO 	glsl
IF NOT EXIST %TARGET_LOC%\glsl MKDIR %TARGET_LOC%\glsl
CALL %SHADERC% ^
--type fragment ^
--platform linux ^
--profile 300_es ^
--varyingdef %SOURCE_LOC%\varying.def.sc ^
-f %SOURCE_LOC%\fs_%SHADER_NAME%.sc ^
-o %TARGET_LOC%\glsl\fs_%SHADER_NAME%.bin

ECHO 	spirv
IF NOT EXIST %TARGET_LOC%\spirv MKDIR %TARGET_LOC%\spirv
CALL %SHADERC% ^
--type fragment ^
--platform linux ^
--profile spirv ^
--varyingdef %SOURCE_LOC%\varying.def.sc ^
-f %SOURCE_LOC%\fs_%SHADER_NAME%.sc ^
-o %TARGET_LOC%\spirv\fs_%SHADER_NAME%.bin

ECHO 	dx9
IF NOT EXIST %TARGET_LOC%\dx9 MKDIR %TARGET_LOC%\dx9
CALL %SHADERC% ^
--type fragment ^
--platform windows ^
--profile ps_3_0 ^
-O 3 ^
--varyingdef %SOURCE_LOC%\varying.def.sc ^
-f %SOURCE_LOC%\fs_%SHADER_NAME%.sc ^
-o %TARGET_LOC%\dx9\fs_%SHADER_NAME%.bin

ECHO 	dx11
IF NOT EXIST %TARGET_LOC%\dx11 MKDIR %TARGET_LOC%\dx11
CALL %SHADERC% ^
--type fragment ^
--platform windows ^
--profile ps_4_0 ^
-O 3 ^
--varyingdef %SOURCE_LOC%\varying.def.sc ^
-f %SOURCE_LOC%\fs_%SHADER_NAME%.sc ^
-o %TARGET_LOC%\dx11\fs_%SHADER_NAME%.bin

ECHO 	metal
IF NOT EXIST %TARGET_LOC%\metal MKDIR %TARGET_LOC%\metal
CALL %SHADERC% ^
--type fragment ^
--platform ios ^
--profile metal ^
--varyingdef %SOURCE_LOC%\varying.def.sc ^
-f %SOURCE_LOC%\fs_%SHADER_NAME%.sc ^
-o %TARGET_LOC%\metal\fs_%SHADER_NAME%.bin


EXIT /B 0
