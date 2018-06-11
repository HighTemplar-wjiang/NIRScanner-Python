cmake_minimum_required(VERSION 3.9)
# Project name
project(nirs_scanner)
set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ludev")

# Sources
include_directories(./)
file(GLOB SOURCES_C "./*.c")
file(GLOB SOURCES_CPP "./*.cpp")

# Library
add_library(objc STATIC ${SOURCES_C})
add_library(objcpp STATIC ${SOURCES_CPP})

# Executable
add_executable(main main.cpp)

# Link
target_link_libraries(objc objcpp udev)
target_link_libraries(objcpp objc udev)
target_link_libraries(main objc objcpp udev)