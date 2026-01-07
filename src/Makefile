# Makefile for compiling main.cpp with SDL3 (macOS/Unix version)

# Compiler settings
CXX = clang++
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Directories
SRC_DIR = .
BUILD_DIR = .
SDL_DIR = ../SDL
SDL_INCLUDE = $(SDL_DIR)/include
# SDL build config directory (may vary by build type)
SDL_BUILD_CONFIG = $(SDL_DIR)/build/include-config-release
SDL_BUILD_CONFIG_ALT = $(SDL_DIR)/build/include-config-RelWithDebInfo
SDL_LIB_DIR = $(SDL_DIR)/build

# Include paths (try both possible config directories)
# Add current directory so subdirectories can find each other's headers

# Auto-detect FreeType paths (works across different systems)
# Try freetype-config first (macOS Homebrew), then pkg-config (Linux), then fallback paths
FREETYPE_CFLAGS := $(shell freetype-config --cflags 2>/dev/null || pkg-config --cflags freetype2 2>/dev/null || echo "-I/opt/homebrew/opt/freetype/include/freetype2 -I/usr/local/include/freetype2 -I/usr/include/freetype2")
FREETYPE_LDFLAGS := $(shell freetype-config --libs 2>/dev/null | sed 's/-lfreetype//' || pkg-config --libs-only-L freetype2 2>/dev/null || echo "-L/opt/homebrew/opt/freetype/lib -L/usr/local/lib -L/usr/lib")
FREETYPE_LIBS := -lfreetype

INCLUDES = -I. -I$(SDL_INCLUDE) -I$(SDL_BUILD_CONFIG) -I$(SDL_BUILD_CONFIG_ALT) $(FREETYPE_CFLAGS)

# Library paths and libraries
# On macOS, SDL3 builds as libSDL3.0.dylib, but we link with -lSDL3

LDFLAGS = -L$(SDL_LIB_DIR) -Wl,-rpath,$(SDL_LIB_DIR) $(FREETYPE_LDFLAGS)
LIBS = -lSDL3 $(FREETYPE_LIBS)

# Source and output files
SRC = main.cpp core/textures.cpp world/tiles.cpp world/objects.cpp items/resources.cpp definitions/tile_definitions.cpp definitions/object_definitions.cpp definitions/resource_definitions.cpp world/map_generation.cpp world/planet.cpp world/planet_tree.cpp core/camera.cpp core/input_manager.cpp entities/player.cpp definitions/frame_definitions.cpp states/GameContext.cpp states/MortalState.cpp states/GodState.cpp ui/hud.cpp ui/text_renderer.cpp core/Animations.cpp world/BigBangEngine.cpp
OBJ = $(BUILD_DIR)/main.o $(BUILD_DIR)/textures.o $(BUILD_DIR)/tiles.o $(BUILD_DIR)/objects.o $(BUILD_DIR)/resources.o $(BUILD_DIR)/tile_definitions.o $(BUILD_DIR)/object_definitions.o $(BUILD_DIR)/resource_definitions.o $(BUILD_DIR)/map_generation.o $(BUILD_DIR)/planet.o $(BUILD_DIR)/planet_tree.o $(BUILD_DIR)/camera.o $(BUILD_DIR)/input_manager.o $(BUILD_DIR)/player.o $(BUILD_DIR)/frame_definitions.o $(BUILD_DIR)/GameContext.o $(BUILD_DIR)/MortalState.o $(BUILD_DIR)/GodState.o $(BUILD_DIR)/hud.o $(BUILD_DIR)/text_renderer.o $(BUILD_DIR)/Animations.o $(BUILD_DIR)/BigBangEngine.o
TARGET = $(BUILD_DIR)/game

# Default target
all: $(TARGET)

# Link the object files into the final executable
# Link the object files into the final executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) $(LIBS) -o $(TARGET)
	@echo Build complete: $(TARGET)

# Compile source to object files
$(BUILD_DIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/textures.o: core/textures.cpp core/textures.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c core/textures.cpp -o $(BUILD_DIR)/textures.o

$(BUILD_DIR)/tiles.o: world/tiles.cpp world/tiles.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c world/tiles.cpp -o $(BUILD_DIR)/tiles.o

$(BUILD_DIR)/objects.o: world/objects.cpp world/objects.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c world/objects.cpp -o $(BUILD_DIR)/objects.o

$(BUILD_DIR)/resources.o: items/resources.cpp items/resources.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c items/resources.cpp -o $(BUILD_DIR)/resources.o

$(BUILD_DIR)/tile_definitions.o: definitions/tile_definitions.cpp definitions/tile_definitions.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c definitions/tile_definitions.cpp -o $(BUILD_DIR)/tile_definitions.o

$(BUILD_DIR)/object_definitions.o: definitions/object_definitions.cpp definitions/object_definitions.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c definitions/object_definitions.cpp -o $(BUILD_DIR)/object_definitions.o

$(BUILD_DIR)/resource_definitions.o: definitions/resource_definitions.cpp definitions/resource_definitions.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c definitions/resource_definitions.cpp -o $(BUILD_DIR)/resource_definitions.o

$(BUILD_DIR)/map_generation.o: world/map_generation.cpp world/map_generation.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c world/map_generation.cpp -o $(BUILD_DIR)/map_generation.o

$(BUILD_DIR)/planet.o: world/planet.cpp world/planet.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c world/planet.cpp -o $(BUILD_DIR)/planet.o

$(BUILD_DIR)/planet_tree.o: world/planet_tree.cpp world/planet_tree.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c world/planet_tree.cpp -o $(BUILD_DIR)/planet_tree.o

$(BUILD_DIR)/camera.o: core/camera.cpp core/camera.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c core/camera.cpp -o $(BUILD_DIR)/camera.o

$(BUILD_DIR)/input_manager.o: core/input_manager.cpp core/input_manager.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c core/input_manager.cpp -o $(BUILD_DIR)/input_manager.o

$(BUILD_DIR)/player.o: entities/player.cpp entities/player.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c entities/player.cpp -o $(BUILD_DIR)/player.o

$(BUILD_DIR)/frame_definitions.o: definitions/frame_definitions.cpp definitions/frame_definitions.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c definitions/frame_definitions.cpp -o $(BUILD_DIR)/frame_definitions.o

$(BUILD_DIR)/GameContext.o: states/GameContext.cpp states/GameContext.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c states/GameContext.cpp -o $(BUILD_DIR)/GameContext.o

$(BUILD_DIR)/MortalState.o: states/MortalState.cpp states/MortalState.h states/BaseState.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c states/MortalState.cpp -o $(BUILD_DIR)/MortalState.o

$(BUILD_DIR)/GodState.o: states/GodState.cpp states/GodState.h states/BaseState.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c states/GodState.cpp -o $(BUILD_DIR)/GodState.o

$(BUILD_DIR)/hud.o: ui/hud.cpp ui/hud.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c ui/hud.cpp -o $(BUILD_DIR)/hud.o

$(BUILD_DIR)/text_renderer.o: ui/text_renderer.cpp ui/text_renderer.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c ui/text_renderer.cpp -o $(BUILD_DIR)/text_renderer.o

$(BUILD_DIR)/Animations.o: core/Animations.cpp core/Animations.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c core/Animations.cpp -o $(BUILD_DIR)/Animations.o

$(BUILD_DIR)/BigBangEngine.o: world/BigBangEngine.cpp world/BigBangEngine.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c world/BigBangEngine.cpp -o $(BUILD_DIR)/BigBangEngine.o

# Clean build artifacts
clean:
	rm -f $(BUILD_DIR)/main.o
	rm -f $(BUILD_DIR)/textures.o
	rm -f $(BUILD_DIR)/tiles.o
	rm -f $(BUILD_DIR)/objects.o
	rm -f $(BUILD_DIR)/resources.o
	rm -f $(BUILD_DIR)/tile_definitions.o
	rm -f $(BUILD_DIR)/object_definitions.o
	rm -f $(BUILD_DIR)/resource_definitions.o
	rm -f $(BUILD_DIR)/map_generation.o
	rm -f $(BUILD_DIR)/planet.o
	rm -f $(BUILD_DIR)/planet_tree.o
	rm -f $(BUILD_DIR)/camera.o
	rm -f $(BUILD_DIR)/input_manager.o
	rm -f $(BUILD_DIR)/player.o
	rm -f $(BUILD_DIR)/frame_definitions.o
	rm -f $(BUILD_DIR)/GameContext.o
	rm -f $(BUILD_DIR)/MortalState.o
	rm -f $(BUILD_DIR)/GodState.o
	rm -f $(BUILD_DIR)/hud.o
	rm -f $(BUILD_DIR)/text_renderer.o
	rm -f $(BUILD_DIR)/Animations.o
	rm -f $(BUILD_DIR)/BigBangEngine.o
	rm -f $(TARGET)
	@echo Clean complete

# Phony targets
.PHONY: all clean