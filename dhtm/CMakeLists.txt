target_sources(dhtmlib
	PRIVATE
		terminal.cpp
		terminalController.cpp
	PUBLIC
		terminal.hpp
		terminalController.hpp
		stringConstants.hpp
)

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)
find_package(Curses REQUIRED) 

find_library(panel REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(dhtmlib panel xtensor ${CURSES_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(dhtmlib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
