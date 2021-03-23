
@ECHO OFF
SETLOCAL
CD /D %~dp0

SET SHADERC="..\..\..\..\Dependencies\tools\bgfx\shadercRelease.exe"
SET SHADER_TEMP=shader.temp


CALL :COMPILE_VERTEX_SHADER color
CALL :COMPILE_VERTEX_SHADER tex
CALL :COMPILE_VERTEX_SHADER phong
CALL :COMPILE_VERTEX_SHADER phong_skinned
CALL :COMPILE_VERTEX_SHADER phong_instanced
CALL :COMPILE_VERTEX_SHADER text

CALL :COMPILE_FRAGMENT_SHADER color
CALL :COMPILE_FRAGMENT_SHADER tex
CALL :COMPILE_FRAGMENT_SHADER phong
CALL :COMPILE_FRAGMENT_SHADER text


IF EXIST %SHADER_TEMP% DEL %SHADER_TEMP%

PAUSE
EXIT /B 0


:COMPILE_VERTEX_SHADER
ECHO vs_%~1.sc

ECHO 	glsl
CALL %SHADERC% ^
--type vertex ^
--platform windows ^
--profile 300_es ^
--bin2c vs_%~1_glsl ^
--varyingdef varying.def.sc ^
-f vs_%~1.sc ^
-o %SHADER_TEMP%
TYPE %SHADER_TEMP% > ..\vs_%~1.h

ECHO 	spv
CALL %SHADERC% ^
--type vertex ^
--platform windows ^
--profile spirv ^
--bin2c vs_%~1_spv ^
--varyingdef varying.def.sc ^
-f vs_%~1.sc ^
-o %SHADER_TEMP%
TYPE %SHADER_TEMP% >> ..\vs_%~1.h

ECHO 	dx9
CALL %SHADERC% ^
--type vertex ^
--platform windows ^
--profile vs_3_0 ^
-O 3 ^
--bin2c vs_%~1_dx9 ^
--varyingdef varying.def.sc ^
-f vs_%~1.sc ^
-o %SHADER_TEMP%
TYPE %SHADER_TEMP% >> ..\vs_%~1.h

ECHO 	dx11
CALL %SHADERC% ^
--type vertex ^
--platform windows ^
--profile vs_4_0 ^
-O 3 ^
--bin2c vs_%~1_dx11 ^
--varyingdef varying.def.sc ^
-f vs_%~1.sc ^
-o %SHADER_TEMP%
TYPE %SHADER_TEMP% >> ..\vs_%~1.h

ECHO 	metal
CALL %SHADERC% ^
--type vertex ^
--platform ios ^
--profile metal ^
--bin2c vs_%~1_mtl ^
--varyingdef varying.def.sc ^
-f vs_%~1.sc ^
-o %SHADER_TEMP%
TYPE %SHADER_TEMP% >> ..\vs_%~1.h

ECHO extern const uint8_t* vs_%~1_pssl;>> ..\vs_%~1.h
ECHO extern const uint32_t vs_%~1_pssl_size;>> ..\vs_%~1.h

EXIT /B 0

:COMPILE_FRAGMENT_SHADER
ECHO fs_%~1.sc

ECHO 	glsl
CALL %SHADERC% ^
--type fragment ^
--platform windows ^
--profile 300_es ^
--bin2c fs_%~1_glsl ^
--varyingdef varying.def.sc ^
-f fs_%~1.sc ^
-o %SHADER_TEMP%
TYPE %SHADER_TEMP% > ..\fs_%~1.h

ECHO 	spv
CALL %SHADERC% ^
--type fragment ^
--platform windows ^
--profile spirv ^
--bin2c fs_%~1_spv ^
--varyingdef varying.def.sc ^
-f fs_%~1.sc ^
-o %SHADER_TEMP%
TYPE %SHADER_TEMP% >> ..\fs_%~1.h

ECHO 	dx9
CALL %SHADERC% ^
--type fragment ^
--platform windows ^
--profile ps_3_0 ^
-O 3 ^
--bin2c fs_%~1_dx9 ^
--varyingdef varying.def.sc ^
-f fs_%~1.sc ^
-o %SHADER_TEMP%
TYPE %SHADER_TEMP% >> ..\fs_%~1.h

ECHO 	dx11
CALL %SHADERC% ^
--type fragment ^
--platform windows ^
--profile ps_4_0 ^
-O 3 ^
--bin2c fs_%~1_dx11 ^
--varyingdef varying.def.sc ^
-f fs_%~1.sc ^
-o %SHADER_TEMP%
TYPE %SHADER_TEMP% >> ..\fs_%~1.h

ECHO 	metal
CALL %SHADERC% ^
--type fragment ^
--platform ios ^
--profile metal ^
--bin2c fs_%~1_mtl ^
--varyingdef varying.def.sc ^
-f fs_%~1.sc ^
-o %SHADER_TEMP%
TYPE %SHADER_TEMP% >> ..\fs_%~1.h

ECHO extern const uint8_t* fs_%~1_pssl;>> ..\fs_%~1.h
ECHO extern const uint32_t fs_%~1_pssl_size;>> ..\fs_%~1.h

EXIT /B 0
