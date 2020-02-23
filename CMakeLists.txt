cmake_minimum_required(VERSION 3.1)
project(welt)
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

# $ sudo apt-get install libsdl2-image-dev
# $ sudo apt-get install libsdl2-dev

# Make sure the output binary will be placed in the bin directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Find SDL2 and SDL2_image
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
find_package(SDL2 REQUIRED COMPONENTS main)
find_package(SDL2_image REQUIRED COMPONENTS main)

# Define executable target
include_directories(${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${SDL2main_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR})

add_subdirectory(examples/01-Wolf_and_Sheep)