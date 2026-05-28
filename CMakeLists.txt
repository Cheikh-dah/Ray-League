cmake_minimum_required(VERSION 3.16)
project(RocketLeague VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(RocketLeague main.cpp)

target_link_libraries(RocketLeague PRIVATE raylib)

# Force the linker to use the correct Windows OpenGL library name
if(WIN32)
    target_link_libraries(RocketLeague PRIVATE opengl32)
endif()
