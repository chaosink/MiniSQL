cmake_minimum_required(VERSION 3.0)

project(MiniSQL VERSION 0.1 LANGUAGES CXX)

file(GLOB SOURCE_FILES
	src/core/*.cpp
)

file(GLOB HEADER_FILES
	src/core/*.h
)

include_directories(
	src/core
)

add_library(MiniSQL_core
	${SOURCE_FILES}
	${HEADER_FILES}
)

add_executable(MiniSQL
	src/MiniSQL/MiniSQL.cpp
)
target_link_libraries(MiniSQL
	MiniSQL_core
)
