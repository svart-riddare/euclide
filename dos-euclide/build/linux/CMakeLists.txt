cmake_minimum_required(VERSION 2.8)

# Source files

set(SOURCES
	../../source/colors.h
	../../source/console.cpp
	../../source/console.h
	../../source/console-linux.cpp
	../../source/console-linux.h
	../../source/forsythe.cpp
	../../source/forsythe.h
	../../source/includes.h
	../../source/main.cpp
	../../source/resources.h
	../../source/strings.cpp
	../../source/strings.h
	../../source/timer.cpp
	../../source/timer.h
)

# Project definition

project(dos-euclide)

add_executable(dos-euclide ${SOURCES})

set_target_properties(dos-euclide PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../bin)

# Add dependency on euclide library

add_subdirectory(../../../euclide/build/linux ../../../euclide/build/linux)
add_dependencies(dos-euclide euclide)
target_link_libraries(dos-euclide euclide)

# Compile options

add_definitions(-DNDEBUG -DEUCLIDE_LINUX -g -O3 -Wall -Wextra -Wno-deprecated)
include_directories(../../../euclide/interface)

# Dependent on ncurses library

target_link_libraries(dos-euclide ncursesw)

