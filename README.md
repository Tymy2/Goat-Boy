# Goat-Boy
Simple (not complete) emulator for GameBoy made for fun.

## Introduction

This is a personal project that im going to use as a base to learn cpp so expect bad practices of the language.

## Building

I dont really know a lot about `CMake` so the work around i have to build the project is the next one.

First, make sure that the `libs` folder contains the git repository of `SDL 2`, then just run the next pair of commands from the root directory:

```bash
#Setting up
mkdir build
mkdir libs
cd libs
git clone -b release-2.30.x --depth 1 "https://github.com/libsdl-org/SDL.git"

#Building
cmake -S . -B build
cmake --build build
```

After that, if everything is alright, you can find the executable under `build/Debug/GoatBoy.exe`.

## How to use

To run the program you can do this:

```sh
GoatBoy.exe <game_rom>
```

And thats it!

## Notes

If you had the horrible idea of trying out this project and you find some problems, you can submit them to me and i might fix them or not, depends on my mood.

