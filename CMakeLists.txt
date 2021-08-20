cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Include local cmake modules
include(pico_sdk)

project(gleos VERSION 1.0 LANGUAGES C CXX ASM)

# Request the latest C++ standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Initialize the SDK
pico_sdk_init()

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(firmware)
