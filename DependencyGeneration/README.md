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
## BGFX + BIMG + BX
    repository: https://github.com/bkaradzic/bgfx
    repository: https://github.com/bkaradzic/bimg
    repository: https://github.com/bkaradzic/bx
    script: bgfx.bat
    Rendering backend wrapper framework.
## BOOST
    repository: https://github.com/boostorg/boost
    script: boost.bat
    General purpose libraries.
## FREETYPE
    repository: https://gitlab.freedesktop.org/freetype/freetype
    script: TBD
    Font loading.
## GLM
    repository: https://github.com/g-truc/glm
    script: TBD
    Math library.
## DEAR-IMGUI
    repository: https://github.com/ocornut/imgui
    script: TBD, NOTE: Some files may be modified!
    GUI library
## KTX-SOFTWARE
    repository: https://github.com/KhronosGroup/KTX-Software
    script: TBD
    Tools for creating ktx textures.
    Not currently required for building any part of the engine.
## SDL2
    repository: https://github.com/libsdl-org/SDL
    script: sdl.bat
    Input and window management.
## SOLOUD
    repository: https://github.com/jarikomppa/soloud
    script: soloud.bat
    Audio engine.

## ZLIB
    ?
## BCM2835
    ?
## IRRXML
    ?
## STEAM / GAME-NETWORKING-SOCKETS
    ?
