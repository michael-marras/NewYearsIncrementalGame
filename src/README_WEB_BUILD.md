# Web Build Instructions

## Overview

This directory contains the Emscripten build configuration for compiling the game to WebAssembly (WASM) for running in web browsers.

## Files

- `Makefile.emscripten` - Emscripten-specific Makefile
- `shell.html` - HTML shell template for Emscripten output
- `../build_web.sh` - Helper build script (in parent directory)

## Quick Build

```bash
# From project root:
./build_web.sh

# Or manually from src/:
make -f Makefile.emscripten
```

## Output

After building, the `build_emscripten/` directory will contain:

- `index.html` - Main HTML file (your game!)
- `index.wasm` - WebAssembly binary
- `index.js` - JavaScript loader
- `res/` - Preloaded game assets
- `fonts/` - Preloaded fonts

## Creating Distribution ZIP

```bash
make -f Makefile.emscripten package
```

This creates `game_web.zip` in the `src/` directory with all necessary files.

## Requirements

1. **Emscripten** - See `../BUILD_WEB.md` for installation
2. **SDL3 built for Emscripten** - See `../BUILD_WEB.md` for instructions

## Notes

- Assets are preloaded using `--preload-file`, so they're embedded in the output
- The game uses `sASYNCIFY` for async operations (file loading, etc.)
- Memory settings can be adjusted in `Makefile.emscripten` if needed
- The shell.html template provides a basic HTML structure - you can customize it

## Troubleshooting

See `../BUILD_WEB.md` for detailed troubleshooting.
