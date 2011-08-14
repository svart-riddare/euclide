cmake_minimum_required(VERSION 2.8)

# Source files

set(SOURCES
	../../source/assignations.cpp
	../../source/assignations.h
	../../source/board.cpp
	../../source/board.h
	../../source/captures.cpp
	../../source/captures.h
	../../source/constraints.cpp
	../../source/constraints.h
	../../source/destinations.cpp
	../../source/destinations.h
	../../source/euclide.cpp
	../../source/hashtables.cpp
	../../source/hashtables.h
	../../source/implications.cpp
	../../source/implications.h
	../../source/includes.h
	../../source/moves.cpp
	../../source/moves.h
	../../source/obstructions.cpp
	../../source/obstructions.h
	../../source/partitions.cpp
	../../source/partitions.h
	../../source/pieces.cpp
	../../source/pieces.h
	../../source/position.cpp
	../../source/position.h
	../../source/partitions.cpp
	../../source/partitions.h
	../../source/pieces.cpp
	../../source/pieces.h
	../../source/position.cpp
	../../source/position.h
	../../source/problem.cpp
	../../source/problem.h
	../../source/targets.cpp
	../../source/targets.h
)

set(CONSTANTS
	../../source/constants/constants.h
	../../source/constants/copyright.cpp
	../../source/constants/infinity.cpp
)

set(ENUMERATIONS
	../../source/enumerations/colors.h
	../../source/enumerations/enumerations.h
	../../source/enumerations/errors.cpp
	../../source/enumerations/errors.h
	../../source/enumerations/glyphs.h
	../../source/enumerations/men.h
	../../source/enumerations/squares.h
)

set(TABLES
	../../source/tables/check-tables.cpp
	../../source/tables/constraint-tables.cpp
	../../source/tables/movement-tables.cpp
	../../source/tables/obstruction-tables.cpp
	../../source/tables/tables.h
)

set(UTILITIES
	../../source/utilities/algorithm.h
	../../source/utilities/array.h
	../../source/utilities/bitset.h
	../../source/utilities/matrix.h
	../../source/utilities/pointer.h
	../../source/utilities/predicates.h
	../../source/utilities/tag.h
	../../source/utilities/vector.h
)

# Project definition

project(euclide)

add_library(euclide ${SOURCES} ${CONSTANTS} ${ENUMERATIONS} ${TABLES} ${UTILITIES} ../../interface/euclide.h)

set_target_properties(euclide PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../../bin/)

# Boost requirement

find_package(Boost)

# Compilation flags

add_definitions(-DNDEBUG -O3 -g -Wall -Wextra -Wno-strict-aliasing)


