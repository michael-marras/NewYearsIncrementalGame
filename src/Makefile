# Compiler
CXX := g++
CC := gcc

# Compiler flags
CXXFLAGS := -Wall -Wextra -std=c++17

# Set PKG_CONFIG_PATH to use local SDL3 build
export PKG_CONFIG_PATH := $(shell pwd)/../SDL/build:$(PKG_CONFIG_PATH)

# SDL3 flags from pkg-config
SDL_LIBS := $(shell PKG_CONFIG_PATH=../SDL/build pkg-config --libs sdl3)
SDL_CFLAGS := $(shell PKG_CONFIG_PATH=../SDL/build pkg-config --cflags sdl3)

# OpenGL framework (macOS)
LDFLAGS := -framework OpenGL

# Target executable
TARGET := game

# Source files
SRC_CPP := main.cpp textures.cpp tiles.cpp tile_definitions.cpp camera.cpp

# Object files
OBJS := main.o textures.o tiles.o tile_definitions.o camera.o

# Default rule
all: $(TARGET)

# Compile C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c $< -o $@

# Compile main.cpp (explicit rule for clarity)
main.o: main.cpp textures.h tiles.h tile_definitions.h constants.h camera.h states/GameContext.h
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c main.cpp -o main.o

# Compile textures.cpp
textures.o: textures.cpp textures.h
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c textures.cpp -o textures.o

# Compile tiles.cpp
tiles.o: tiles.cpp tiles.h
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c tiles.cpp -o tiles.o

# Compile tile_definitions.cpp
tile_definitions.o: tile_definitions.cpp tile_definitions.h
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c tile_definitions.cpp -o tile_definitions.o

# Compile camera.cpp
camera.o: camera.cpp camera.h
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c camera.cpp -o camera.o

# Link everything together
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(SDL_LIBS) $(LDFLAGS)

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
