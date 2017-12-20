SpehsEngine was initially built around the game Spehs by Juuso Turunen and Teo Hiltunen.
www.spehs.com

SpehsEngine setup:
Run the setROOT bat file to set the SPEHENGINE_ROOT environment variable.
Acquire the required dependencies. TODO...
Build the wanted libraries.

Project setup and use:
Additional include directory: $(SPEHSENGINE_ROOT)
Additional library directory: $(SPEHSENGINE_ROOT)/bin/$(Platform)/$(Configuration)/$(PlatformToolset)
Spehs libraries must be initialized in a set order by calling spehs::<libraryNamespace>::initialize(), #include "SpehsEngine/<LibraryName>/<LibraryName>.h".
