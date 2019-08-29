cmake_minimum_required(VERSION 3.10)

# Run with cmake -DCMAKE_PREFIX_PATH=~/miniconda3 ..

include_directories( ~/miniconda3/include)
include_directories( ~/workspace/tiny_htm)
include_directories( ~/workspace/plot_htm)

project(phtest)
find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)
find_package(Curses REQUIRED)
find_library(panel REQUIRED)
add_executable(phtest phtest.cpp)
target_link_libraries(phtest panel ${CURSES_LIBRARY})

project(wintest)
find_package(Curses REQUIRED)
add_executable(wintest wintest.cpp)
target_link_libraries(wintest ${CURSES_LIBRARY})
