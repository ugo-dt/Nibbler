#!/bin/bash

set -eu

# Usage/help
show_help() {
    cat <<EOF
Usage: $0 [OPTIONS]

Options:
  -f, --force           Force rebuild for all targets (remove build dirs)
  --force=LIST          Comma-separated list of targets to force (allegro,sdl3,glfw)
  -h, --help            Show this help message

Examples:
  $0 --force=allegro      # only remove Allegro build dir and rebuild it
  $0 -f                   # remove all build dirs and rebuild everything
EOF
}

# Parse arguments
FORCE_ALL=0
FORCE_LIST=""
while [ "$#" -gt 0 ]; do
    case "$1" in
        -f|--force)
            FORCE_ALL=1
            shift
            ;;
        --force=*)
            FORCE_LIST="${1#--force=}"
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        --)
            shift
            break
            ;;
        -*)
            echo "Unknown option: $1" >&2
            exit 2
            ;;
        *)
            break
            ;;
    esac
done

# Helper: should_force <target>
# returns 0 (true) if target should be forced
should_force() {
    local target="$1"
    if [ "$FORCE_ALL" -eq 1 ]; then
        return 0
    fi
    if [ -z "$FORCE_LIST" ]; then
        return 1
    fi
    # split FORCE_LIST by comma
    IFS=',' read -r -a arr <<< "$FORCE_LIST"
    for t in "${arr[@]}"; do
        if [ "$t" = "$target" ]; then
            return 0
        fi
    done
    return 1
}

# Allegro
ALLEGRO_BUILD_DIR="./NibblerAllegro/allegro/build"
if [ -d "$ALLEGRO_BUILD_DIR" ]; then
    if should_force allegro; then
        echo "Allegro: force enabled, removing $ALLEGRO_BUILD_DIR"
        rm -rf "$ALLEGRO_BUILD_DIR"
    else
        echo "Allegro: Already configured."
    fi
fi
if [ ! -d "$ALLEGRO_BUILD_DIR" ]; then
    cmake -S NibblerAllegro/allegro/ -B NibblerAllegro/allegro/build -DPREFER_STATIC_DEPS=ON -DWANT_STATIC_RUNTIME=ON
    cmake --build NibblerAllegro/allegro/build
    echo "Allegro: configured."
fi

# SDL3
SDL3_BUILD_DIR="./NibblerSDL/SDL3/build"
if [ -d "$SDL3_BUILD_DIR" ]; then
    if should_force sdl3; then
        echo "SDL3: force enabled, removing $SDL3_BUILD_DIR"
        rm -rf "$SDL3_BUILD_DIR"
    else
        echo "SDL3: Already configured."
    fi
fi
if [ ! -d "$SDL3_BUILD_DIR" ]; then
    cmake -S NibblerSDL/SDL3/ -B NibblerSDL/SDL3/build
    cmake --build NibblerSDL/SDL3/build
    echo "SDL3: configured."
fi

# glfw
GLFW_BUILD_DIR="./NibblerGLFW/glfw/build"
if [ -d "$GLFW_BUILD_DIR" ]; then
    if should_force glfw; then
        echo "glfw: force enabled, removing $GLFW_BUILD_DIR"
        rm -rf "$GLFW_BUILD_DIR"
    else
        echo "glfw: Already configured."
    fi
fi
if [ ! -d "$GLFW_BUILD_DIR" ]; then
    cmake -S NibblerGLFW/glfw/ -B NibblerGLFW/glfw/build -DBUILD_SHARED_LIBS=ON
    cmake --build NibblerGLFW/glfw/build
    echo "glfw: configured."
fi
