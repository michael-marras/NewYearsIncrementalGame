#!/bin/bash

# Script to check and build FreeType port for Emscripten

FREETYPE_PATH="$HOME/.emscripten_cache/ports-builds/freetype/include/ft2build.h"

echo "Checking for FreeType port..."

if [ -f "$FREETYPE_PATH" ]; then
    echo "✓ FreeType port is already built!"
    echo "  Found at: $FREETYPE_PATH"
    exit 0
else
    echo "✗ FreeType port not found!"
    echo ""
    echo "Building FreeType port now..."
    echo "  (This may take a few minutes)"
    echo ""
    
    if command -v embuilder &> /dev/null; then
        embuilder build freetype
        if [ $? -eq 0 ]; then
            echo ""
            echo "✓ FreeType port built successfully!"
            echo "  Headers installed at: $HOME/.emscripten_cache/ports-builds/freetype/include"
            exit 0
        else
            echo ""
            echo "✗ Failed to build FreeType port!"
            echo "  Please check the error messages above."
            exit 1
        fi
    else
        echo "✗ embuilder not found!"
        echo ""
        echo "Please make sure Emscripten is activated:"
        echo "  source path/to/emsdk/emsdk_env.sh"
        exit 1
    fi
fi

