# Spehs Engine
SpehsEngine (Spehs libraries) was initially built around the game Spehs by Juuso Turunen and Teo Hiltunen.
www.spehs.com

## SpehsEngine setup
Run the setROOT bat file to set the SPEHENGINE_ROOT environment variable.
Acquire the required dependencies. TODO...
Build the wanted libraries.

## Project setup and use
Additional include directory: $(SPEHSENGINE_ROOT)
Additional library directory: $(SPEHSENGINE_ROOT)/bin/$(Platform)/$(Configuration)/$(PlatformToolset)
Spehs libraries must be initialized in a set order by calling spehs::<libraryNamespace>::initialize(), #include "SpehsEngine/<LibraryName>/<LibraryName>.h".

## Spehs libraries

### Core
Minimal, generic functionality that can be useful in various projects.

### Rendering
Window management, opengl based text and polygon rendering.
Uses SDL for window management.
Uses Freetype for text rendering.

### Input
Input detection library. Supported devices: mouse, keyboard and joystics/gamepads.
Uses SDL.

### Audio
Audio playback. Supported formats: .ogg and .wav.

### GUI
A GUI widget library.

### Game
Generalized game object and component library.

### Net
Uses boost's Asio library.

### Sync
Experiemental type synchronization library.

### GPIO
GPIO pin functionality intended for the Raspberry Pi.
Uses the bcm2835 library.
