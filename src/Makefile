# Compiler
CXX := g++

# Compiler flags
CXXFLAGS := -Wall -Wextra -std=c++17

# SDL3 flags from pkg-config
SDL_LIBS := $(shell pkg-config --libs sdl3)
SDL_CFLAGS := $(shell pkg-config --cflags sdl3)

# Target
TARGET := main

# Sources
SRC := main.cpp

# Default rule
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) $(SRC) -o $(TARGET) $(SDL_LIBS)

# Clean rule
clean:
	rm -f $(TARGET)
