cmake_minimum_required(VERSION 3.13)                                                                                                                                                                               

# Run with cmake -DCMAKE_PREFIX_PATH=~/miniconda3 ..

project(dhtmdemo)

set(CMAKE_BUILD_TYPE Debug)

add_library(dhtmlib)
add_library(utilslib)

find_package(xtl REQUIRED)                                                                                                                                                                                      
find_package(xtensor REQUIRED)
find_package(Curses REQUIRED)

find_library(panel REQUIRED)

add_subdirectory(dhtm)

add_executable(dhtmdemo main.cpp)

target_link_libraries(dhtmdemo dhtmlib utilslib panel xtensor ${CURSES_LIBRARY})
