# Compiler
CXX := g++
CC := gcc

# Compiler flags
CXXFLAGS := -Wall -Wextra -std=c++17 -I../glad/include
CFLAGS := -I../glad/include

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
SRC := main.cpp textures.cpp
GLAD_SRC := ../glad/src/glad.c

# Object files
OBJS := main.o textures.o glad.o

# Default rule
all: $(TARGET)

# Compile main.cpp
main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c main.cpp -o main.o

# Compile textures.cpp
textures.o: textures.cpp
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c textures.cpp -o textures.o

# Compile glad.c
glad.o: $(GLAD_SRC)
	$(CC) $(CFLAGS) -c $(GLAD_SRC) -o glad.o

# Link everything together
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(SDL_LIBS) $(LDFLAGS)

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
