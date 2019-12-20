target_sources(dhtmlib
	PRIVATE
		terminalOutput.cpp
		htmController.cpp
	PUBLIC
		terminalOutput.hpp
		htmController.hpp
		stringConstants.hpp
)

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)
find_package(Curses REQUIRED) 

find_library(panel REQUIRED)

target_link_libraries(dhtmlib panel xtensor ${CURSES_LIBRARY})

target_include_directories(dhtmlib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
