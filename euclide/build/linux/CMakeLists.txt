cmake_minimum_required(VERSION 2.8)

# Source files

set(SOURCES
	../../source/cache.cpp
	../../source/cache.h
	../../source/captures.cpp
	../../source/captures.h
	../../source/conditions.cpp
	../../source/conditions.h
	../../source/copyright.cpp
	../../source/definitions.h
	../../source/euclide.cpp
	../../source/game.cpp
	../../source/game.h
	../../source/hashtables.cpp
	../../source/hashtables.h
	../../source/includes.h
	../../source/pieces.cpp
	../../source/pieces.h
	../../source/partitions.cpp
	../../source/partitions.h
	../../source/problem.cpp
	../../source/problem.h
	../../source/targets.cpp
	../../source/targets.h
)

set(TABLES
	../../source/tables/constraint-tables.cpp
	../../source/tables/movement-tables.cpp
	../../source/tables/check-tables.cpp
	../../source/tables/tables.h
)

set(UTILITIES
	../../source/utilities/boost/tribool.hpp
	../../source/utilities/algorithm.h
	../../source/utilities/bitset.h
	../../source/utilities/intrinsics.h
	../../source/utilities/iterator.h
	../../source/utilities/matrix.h
	../../source/utilities/queue.h
)

# Project definition

project(euclide)

add_library(euclide ${SOURCES} ${TABLES} ${UTILITIES} ../../interface/euclide.h)

set_target_properties(euclide PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../../bin/)

# Compilation flags

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -mavx")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-parentheses")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -save-temps -fverbose-asm -masm=intel")

set(CMAKE_CXX_FLAGS_ASAN "${CMAKE_CXX_FLAGS_ASAN} -O1 -g -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS_ASAN "${CMAKE_LINKER_FLAGS_ASAN} -fsanitize=address")

foreach(FILE ${TABLES})
	set_source_files_properties(${FILE} PROPERTIES COMPILE_FLAGS -O0)
endforeach()
