# Makefile for compiling main.cpp with SDL3

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Directories
SRC_DIR = .
BUILD_DIR = .
SDL_DIR = ../SDL
SDL_INCLUDE = $(SDL_DIR)/include
SDL_BUILD_CONFIG = $(SDL_DIR)/build/include-config-release
SDL_LIB_DIR = $(SDL_DIR)/build/Release

# Include paths
INCLUDES = -I$(SDL_INCLUDE) -I$(SDL_BUILD_CONFIG)

# Library paths and libraries
LDFLAGS = -L$(SDL_LIB_DIR)
LIBS = -lSDL3

# Source and output files
SRC = main.cpp textures.cpp tiles.cpp objects.cpp tile_definitions.cpp object_definitions.cpp map_generation.cpp camera.cpp
OBJ = $(BUILD_DIR)/main.o $(BUILD_DIR)/textures.o $(BUILD_DIR)/tiles.o $(BUILD_DIR)/objects.o $(BUILD_DIR)/tile_definitions.o $(BUILD_DIR)/object_definitions.o $(BUILD_DIR)/map_generation.o $(BUILD_DIR)/camera.o
TARGET = $(BUILD_DIR)/game.exe

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) $(LIBS) -o $(TARGET)
	@if exist "$(SDL_LIB_DIR)\SDL3.dll" copy /Y "$(SDL_LIB_DIR)\SDL3.dll" "$(BUILD_DIR)\SDL3.dll"
	@echo Build complete: $(TARGET)

# Compile source to object files
$(BUILD_DIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/textures.o: textures.cpp textures.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c textures.cpp -o $(BUILD_DIR)/textures.o

$(BUILD_DIR)/tiles.o: tiles.cpp tiles.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c tiles.cpp -o $(BUILD_DIR)/tiles.o

$(BUILD_DIR)/objects.o: objects.cpp objects.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c objects.cpp -o $(BUILD_DIR)/objects.o

$(BUILD_DIR)/tile_definitions.o: tile_definitions.cpp tile_definitions.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c tile_definitions.cpp -o $(BUILD_DIR)/tile_definitions.o

$(BUILD_DIR)/object_definitions.o: object_definitions.cpp object_definitions.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c object_definitions.cpp -o $(BUILD_DIR)/object_definitions.o

$(BUILD_DIR)/map_generation.o: map_generation.cpp map_generation.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c map_generation.cpp -o $(BUILD_DIR)/map_generation.o

$(BUILD_DIR)/camera.o: camera.cpp camera.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c camera.cpp -o $(BUILD_DIR)/camera.o

# Clean build artifacts
clean:
	@if exist $(BUILD_DIR)\main.o del /Q $(BUILD_DIR)\main.o
	@if exist $(BUILD_DIR)\textures.o del /Q $(BUILD_DIR)\textures.o
	@if exist $(BUILD_DIR)\tiles.o del /Q $(BUILD_DIR)\tiles.o
	@if exist $(BUILD_DIR)\objects.o del /Q $(BUILD_DIR)\objects.o
	@if exist $(BUILD_DIR)\tile_definitions.o del /Q $(BUILD_DIR)\tile_definitions.o
	@if exist $(BUILD_DIR)\object_definitions.o del /Q $(BUILD_DIR)\object_definitions.o
	@if exist $(BUILD_DIR)\map_generation.o del /Q $(BUILD_DIR)\map_generation.o
	@if exist $(BUILD_DIR)\camera.o del /Q $(BUILD_DIR)\camera.o
	@if exist $(TARGET) del /Q $(TARGET)
	@if exist SDL3.dll del /Q SDL3.dll
	@echo Clean complete

# Phony targets
.PHONY: all clean
