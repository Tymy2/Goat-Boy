# Goat-Boy
Simple (not complete) emulator for GameBoy made for fun.

## Introduction

This is a personal project that im going to use as a base to learn cpp so expect bad practices of the language.

## Building

I am still learning `CMake` so the work around i have to build the project is the next one.

First, make sure that the `libs` folder contains the git repository of `SDL 2`

```bash
cd libs
git clone -b release-2.30.x --depth 1 "https://github.com/libsdl-org/SDL.git"
```
then just run the next pair of commands from the root directory:

```bash
cd ..
cmake -S . -B build
cmake --build build
```

After that, if everything is alright, you can find the executable under `build/Debug/GoatBoy.exe`.

## How to use

To run the program you can do this:

```sh
GoatBoy.exe <game_rom>
```
You can get a little bit of info using the argument `--help`.

And thats it!

## Notes

- If you have any trouble with the emulator you can report it to me and i may fix it. Again, this is a personal project with the objectie of learning so i might not maintain it in the future.
- Project has been only been tested on a `Windows 10` machine using `CL` for the compiler.
