target_sources(utilslib
	PRIVATE
		itcQueue.cpp
	PUBLIC
		itcQueue.hpp
		itcMessage.hpp
)

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)

find_package(Threads REQUIRED)

target_link_libraries(utilslib xtensor ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(utilslib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
