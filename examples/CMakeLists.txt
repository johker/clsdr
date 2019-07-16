cmake_minimum_required(VERSION 3.10)

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)

include_directories( ~/miniconda3/include)
include_directories( ~/workspace/tiny_htm)

project(htmplottest)
add_executable(htmplottest htmplottest.cpp)

