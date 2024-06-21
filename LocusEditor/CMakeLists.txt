cmake_minimum_required(VERSION 3.26)
project(LocusEditor)

set(CMAKE_CXX_STANDARD 17)

# C/CPP Source Files
set (SOURCE_FILES
	src/main.cpp
)

add_executable(LocusEditor ${SOURCE_FILES})

target_link_libraries(LocusEditor PRIVATE LocusEngine)
target_include_directories(LocusEditor PRIVATE ${CMAKE_SOURCE_DIR}/LocusEngine/include)