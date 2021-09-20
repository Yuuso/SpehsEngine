
REM Target directory
REM Should be relative to DependencyGeneration/Scripts directory
SET DEPS_DIR=../Dependencies

REM Microsoft has made it quite complicated to find the location of the correct MSBuild, so it's hardcoded in here for now...
SET MSBUILD="D:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"

REM Assuming cmake is in PATH
SET CMAKE=cmake
