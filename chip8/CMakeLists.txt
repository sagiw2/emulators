cmake_minimum_required(VERSION 3.10)
project(Chip8Inerp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)
file(GLOB SOURCES src/*.cpp)

add_executable(chip8 ${SOURCES})

target_link_libraries(chip8 PRIVATE SDL3::SDL3)