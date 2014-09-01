CXX = gcc
# Set the SDL_LIB and SDL_INCLUDE paths to your install location of SDL
# You may also need to update the linker option rpath, which sets where to look for
# the SDL2 libraries at runtime to match your install
SDL_LIB = -L/usr/local/lib -lSDL2 -lm -Wl,-rpath=/usr/local/lib
SDL_INCLUDE = -I/usr/local/include
# If your compiler is a bit old you may need to use -std=c++0x instead of -std=c++11
CXXFLAGS = -Wall -std=c99 -c $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB)
# The source code and build output directories
SRC = src
BUILD = build
# Executables will be written to top level of repo under out/
TARGET = game

all: clean $(TARGET)

$(TARGET): $(BUILD)/main.o $(BUILD)/raycasting.o $(BUILD)/sdl.o $(BUILD)
	$(CXX) $(BUILD)/raycasting.o $(BUILD)/main.o $(BUILD)/sdl.o $(LDFLAGS) -o $@

$(BUILD)/main.o: $(SRC)/main.c $(BUILD)
	$(CXX) $(CXXFLAGS) -DSDL=1 $< -o $@

$(BUILD)/raycasting.o: $(SRC)/raycasting.c $(BUILD)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD)/sdl.o: $(SRC)/sdl.c $(BUILD)
	$(CXX) $(CXXFLAGS) $< -o $@

# Make sure the build directory exists
$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean
clean:
	-rm -rf $(BUILD)
	-rm -f $(TARGET)
