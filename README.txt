# Spehs Engine
SpehsEngine for game development by Juuso Turunen and Teo Hiltunen.

## SpehsEngine setup
Run the setROOT bat file to set the SPEHENGINE_ROOT environment variable.
Acquire the required dependencies. TODO...
Build the wanted libraries.

## Project setup and use
Additional include directory: $(SPEHSENGINE_ROOT)
Additional library directory: $(SPEHSENGINE_ROOT)/bin/$(Platform)/$(Configuration)/$(PlatformToolset)

## Spehs libraries

### Core
Minimal, generic functionality that can be useful in various projects.
Some libraries come with an additional Visual Studio project file that has the -ARM suffix. They need the **Visual Studio For Linux Development** extension to work.

### Graphics
Window management (using SDL), opengl based text and polygon rendering.

### Input
Input detection library. Supported devices: mouse, keyboard and joystics/gamepads.
Uses SDL.

### Audio
Audio playback. Supported formats: .ogg and .wav.

### GUI
A GUI widget library.

### Net
Uses boost's Asio library.
