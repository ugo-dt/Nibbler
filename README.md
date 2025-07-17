# Nibbler
Cross-platform ultiplayer Snake game.

## Features
- A Snake game.
- 3 cross-platform rendering backends: SDL, GLFW, Allegro5
- Local multiplayer (2 players per machine)
- Wireless multiplayer (both local players can join a server)
- PvP: Snakes can kill each other.
- ImGui debug interface

## Build on Windows, macOS and Linux
Clone and build the project.
```sh
git clone https://github.com/ugo-dt/Nibbler.git --recursive
cd Nibbler
./configure.sh
make
```

## macOS instructions
Set the DYLD_LIBRARY_PATH library before running the game.
In the project root directory, run the following command:
```sh
export DYLD_LIBRARY_PATH=./NibblerSDL/SDL3/build:./NibblerGLFW/glfw/build/src:$DYLD_LIBRARY_PATH
```

## Known bugs
Allegro keyboard layout: wrong keys are being sent for AZERTY keyboards (and probably more).
