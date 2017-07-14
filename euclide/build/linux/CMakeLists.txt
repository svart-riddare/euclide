cmake_minimum_required(VERSION 2.8)

# Source files

set(SOURCES
	../../source/copyright.cpp
	../../source/euclide.cpp
	../../source/includes.h
)

set(CONSTANTS
)

set(ENUMERATIONS
)

set(TABLES
)

set(UTILITIES
)

# Project definition

project(euclide)

add_library(euclide ${SOURCES} ${CONSTANTS} ${ENUMERATIONS} ${TABLES} ${UTILITIES} ../../interface/euclide.h)

set_target_properties(euclide PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../../bin/)

# Compilation flags

add_definitions(-std=c++11 -DNDEBUG -O3 -Wall -Wextra -Wpedantic)


