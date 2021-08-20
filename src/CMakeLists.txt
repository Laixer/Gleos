add_library(gleos INTERFACE)

file(GLOB gleos_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB gleos_driver_SRC "${CMAKE_CURRENT_SOURCE_DIR}/driver/*.cpp")

target_sources(gleos INTERFACE ${gleos_SRC} ${gleos_driver_SRC})
target_include_directories(gleos INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(gleos INTERFACE
    pico_stdlib
    pico_unique_id
    hardware_pwm
    hardware_i2c)