# SpehsEngine Dependencies


# Script requirements
    - Scripts assume that the dependency repositories are already cloned in the same directory as SpehsEngine
    - CMake should be in PATH
    - MSBuild path is hard coded in deps_vars.bat


# How the scripts should work
    - "_gen" scripts should
        - reset and pull git repo
        - generate projects
        - build projects
    - "_copy" scripts should copy includes, dlls, libs, tools and debug symbols to the Dependencies folder


# The dependencies

## ASSIMP
    repository: https://github.com/assimp/assimp
    script: assimp.bat
    3D model loading.
## BGFX
    repository: https://github.com/bkaradzic/bgfx
    script: bgfx.bat
    Rendering backend wrapper.
## BIMG
    repository: https://github.com/bkaradzic/bimg
    script: bgfx.bat
    Image library required for BGFX.
## BX
    repository: https://github.com/bkaradzic/bx
    script: bgfx.bat
    Base library required for BGFX.
## FREETYPE
    repository: https://gitlab.freedesktop.org/freetype/freetype
    script: TBD
    Font loading.
## GLM
    repository: https://github.com/g-truc/glm
    script: TBD
    Math library.
## KTX-SOFTWARE
    repository: https://github.com/KhronosGroup/KTX-Software
    script: TBD
    Tools for creating ktx textures.
    Not currently required for building any part of the engine. Only used by asset building scripts to create cubemaps.
## OGG
    repository: https://github.com/xiph/vorbis
    script: TBD
    Audio loading. (Status TBD with Audio rework...)
## OPENAL
    repository: https://github.com/kcat/openal-soft
    script: TBD
    Audio playback. (Might be replaced in the future...)
## SDL2
    repository: https://github.com/libsdl-org/SDL
    script: TBD
    Input and window management.
## VORBIS
    repository: https://github.com/xiph/vorbis
    script: TBD
    Audio loading. (Status TBD with Audio rework...)

## BOOST
    ?
## ZLIB
    ?
## BCM2835
    ?
## IRRXML
    ?
    
## GLEW
    WILL BE DELETED AFTER GRAPHICS REWORK
## SOIL
    WILL BE DELETED AFTER GRAPHICS REWORK
