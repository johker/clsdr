target_sources(utilslib
	PUBLIC
		itcQueue.hpp
		itcMessage.hpp
)
find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)

find_package(Threads REQUIRED)

set_target_properties(utilslib PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(utilslib PUBLIC xtensor ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(utilslib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
