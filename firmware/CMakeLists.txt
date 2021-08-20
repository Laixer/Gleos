cmake_minimum_required(VERSION 3.15)

# Only build the test target when building for debug.
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_executable(fw_hello fw_hello.cpp)
    target_link_libraries(fw_hello gleos)
    pico_add_extra_outputs(fw_hello)
endif()

add_executable(fw_hydraulic fw_hydraulic.cpp)
target_link_libraries(fw_hydraulic gleos)
pico_add_extra_outputs(fw_hydraulic)

add_executable(fw_imu fw_imu.cpp)
target_link_libraries(fw_imu gleos)
pico_add_extra_outputs(fw_imu)

# add_executable(fw_can fw_can.cpp)
# target_link_libraries(fw_can gleos hardware_spi)
# pico_add_extra_outputs(fw_can)
