cmake_minimum_required(VERSION 2.8)

# Source files

set(SOURCES
	../../source/copyright.cpp
	../../source/definitions.h
	../../source/euclide.cpp
	../../source/includes.h
	../../source/pieces.cpp
	../../source/pieces.h
	../../source/problem.cpp
	../../source/problem.h
)

set(TABLES
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


