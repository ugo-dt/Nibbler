# Nibbler
Cross-platform multiplayer Snake game.

## Features
- A Snake game.
- 3 cross-platform rendering backends: SDL, GLFW, Allegro5
- Local multiplayer (2 players per machine)
- Wireless multiplayer (both local players can join a server)
- PvP: Snakes can kill each other.
- ImGui debug interface

## Build
Clone and build the project on Linux, macOS or Windows (WSL2, using MinGW)

```sh
git clone https://github.com/ugo-dt/Nibbler.git --recursive
cd Nibbler
make
```

## Known bugs
Allegro keyboard layout: wrong keys are being sent for AZERTY keyboards (and probably more).
