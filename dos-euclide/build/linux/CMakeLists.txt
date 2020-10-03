cmake_minimum_required(VERSION 2.8)

# Source files

set(SOURCES
	../../source/colors.h
	../../source/console.cpp
	../../source/console.h
	../../source/console-linux.cpp
	../../source/console-linux.h
	../../source/console-none.cpp
	../../source/console-none.h
	../../source/forsythe.cpp
	../../source/forsythe.h
	../../source/includes.h
	../../source/main.cpp
	../../source/output.cpp
	../../source/output.h
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
include_directories(../../../euclide/interface)
target_link_libraries(dos-euclide euclide)
add_dependencies(dos-euclide euclide)

# Compile options

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -mavx")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-parentheses")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

set(CMAKE_CXX_FLAGS_ASAN "${CMAKE_CXX_FLAGS_ASAN} -O1 -g -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS_ASAN "${CMAKE_LINKER_FLAGS_ASAN} -fsanitize=address")

# Dependent on ncurses library

set(CURSES_NEED_WIDE TRUE)
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

target_link_libraries(dos-euclide ${CURSES_LIBRARIES})
