cmake_minimum_required(VERSION 3.10)

# Run with cmake -DCMAKE_PREFIX_PATH=~/miniconda3 ..

include_directories( ~/miniconda3/include)
include_directories( ~/workspace/tiny_htm)

project(htmplottest)
find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)
find_package(Curses REQUIRED)
add_executable(htmplottest htmplottest.cpp)
target_link_libraries(htmplottest ${CURSES_LIBRARY})

project(ncursestest)
find_package(Curses REQUIRED)
add_executable(ncursestest ncursestest.cpp)
target_link_libraries(ncursestest ${CURSES_LIBRARY})

