# Makefile for compiling main.cpp with SDL3

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Directories
SRC_DIR = .
BUILD_DIR = .
SDL_DIR = ../SDL
SDL_INCLUDE = $(SDL_DIR)/include
SDL_BUILD_CONFIG = $(SDL_DIR)/build/include-config-relwithdebinfo
SDL_LIB_DIR = $(SDL_DIR)/build

# Include paths
INCLUDES = -I$(SDL_INCLUDE) -I$(SDL_BUILD_CONFIG)

# Library paths and libraries
LDFLAGS = -L$(SDL_LIB_DIR) -Wl,-rpath,$(SDL_LIB_DIR)
LIBS = -lSDL3 -framework OpenGL

# Source and output files
SRC = main.cpp textures.cpp tiles.cpp objects.cpp tile_definitions.cpp object_definitions.cpp map_generation.cpp camera.cpp states/GameContext.cpp player.cpp
OBJ = main.o textures.o tiles.o objects.o tile_definitions.o object_definitions.o map_generation.o camera.o states/GameContext.o player.o
TARGET = game

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) $(LIBS) -o $(TARGET)
	@echo Build complete: $(TARGET)

# Compile source to object files
main.o: main.cpp textures.h tiles.h objects.h tile_definitions.h object_definitions.h map_generation.h constants.h camera.h states/GameContext.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c main.cpp -o main.o

textures.o: textures.cpp textures.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c textures.cpp -o textures.o

tiles.o: tiles.cpp tiles.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c tiles.cpp -o tiles.o

objects.o: objects.cpp objects.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c objects.cpp -o objects.o

tile_definitions.o: tile_definitions.cpp tile_definitions.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c tile_definitions.cpp -o tile_definitions.o

object_definitions.o: object_definitions.cpp object_definitions.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c object_definitions.cpp -o object_definitions.o

map_generation.o: map_generation.cpp map_generation.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c map_generation.cpp -o map_generation.o

camera.o: camera.cpp camera.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c camera.cpp -o camera.o

states/GameContext.o: states/GameContext.cpp states/GameContext.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c states/GameContext.cpp -o states/GameContext.o

player.o: player.cpp player.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c player.cpp -o player.o

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET)
	rm -f states/*.o
	@echo Clean complete

# Phony targets
.PHONY: all clean
