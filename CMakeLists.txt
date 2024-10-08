cmake_minimum_required(VERSION 3.1)

project(GoatBoy VERSION 1.0 DESCRIPTION "A Gameboy emulator" LANGUAGES CXX)

add_subdirectory("libs/SDL")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ot /Qpar /std:c++20 /D \"NDEBUG\" /GA /O2 /D_ITERATOR_DEBUG_LEVEL_=0 ")

add_executable(GoatBoy 
		src/Goatboy.cpp 
		src/Gui.cpp 
		src/Ppu.cpp 
		src/Mmu.cpp 
		src/Cpu.cpp 
		src/Joypad.cpp 
		src/Device.cpp 
		src/instructions.cpp
		src/MBC/MBC_0.cpp
		src/MBC/MBC_3.cpp
		)

target_link_libraries(GoatBoy PRIVATE SDL2)
target_link_libraries(GoatBoy PRIVATE SDL2main)

# add SDL.dll to output dir
add_custom_command(
  TARGET GoatBoy POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2>
    $<TARGET_FILE_DIR:GoatBoy>
  VERBATIM
)
