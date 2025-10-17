#!/bin/bash

set -eu

# Allegro
if [ -d "./NibblerAllegro/allegro/build" ]; then
    echo "Allegro: Already configured."
else
    cmake -S NibblerAllegro/allegro/ -B NibblerAllegro/allegro/build -DPREFER_STATIC_DEPS=ON -DWANT_STATIC_RUNTIME=ON
    cmake --build NibblerAllegro/allegro/build
    echo "Allegro: configured."
fi

# SDL3
if [ -d "./NibblerSDL/SDL3/build" ]; then
    echo "SDL3: Already configured."
else
    cmake -S NibblerSDL/SDL3/ -B NibblerSDL/SDL3/build
    cmake --build NibblerSDL/SDL3/build
    echo "SDL3: configured."
fi

# glfw
if [ -d "./NibblerGLFW/glfw/build" ]; then
    echo "glfw: Already configured."
else
    cmake -S NibblerGLFW/glfw/ -B NibblerGLFW/glfw/build -DBUILD_SHARED_LIBS=ON
    cmake --build NibblerGLFW/glfw/build
    echo "glfw: configured."
fi
