cmake_minimum_required(VERSION 2.8)

# Source files

set(SOURCES
	../../source/cache.cpp
	../../source/cache.h
	../../source/copyright.cpp
	../../source/definitions.h
	../../source/euclide.cpp
	../../source/includes.h
	../../source/pieces.cpp
	../../source/pieces.h
	../../source/problem.cpp
	../../source/problem.h
	../../source/targets.cpp
	../../source/targets.h
)

set(TABLES
	../../source/tables/constraint-tables.cpp
	../../source/tables/movement-tables.cpp
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

add_definitions(-std=c++11 -DNDEBUG -O3 -Wall -Wextra)

foreach(FILE ${TABLES})
	set_source_files_properties(${FILE} PROPERTIES COMPILE_FLAGS -O0)
endforeach()

