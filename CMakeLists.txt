cmake_minimum_required(VERSION 3.10)

# Run with cmake -DCMAKE_PREFIX_PATH=~/miniconda3 ..

include_directories( ~/miniconda3/include)
include_directories( ~/workspace/tiny_htm)
include_directories( ~/workspace/plot_htm)

project(dhtm)
find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)
find_package(Curses REQUIRED)
find_library(panel REQUIRED)
file(GLOB SOURCES
	dhtm/*.hpp
	dhtm/*.cpp
)
add_executable(dhtm main.cpp ${SOURCES})
target_link_libraries(dhtm ${CURSES_LIBRARY})
