cmake_minimum_required(VERSION 3.13)

# Initialize the SDK
include(pico_sdk_import.cmake )

project(ultrasonic_sensor C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(ultrasonic_sensor
        main.c
        )

# Enable USB output 
pico_enable_stdio_usb(ultrasonic_sensor 1)

# Enable UART output 
pico_enable_stdio_uart(ultrasonic_sensor 1)


# create map/bin/hex file etc.
pico_add_extra_outputs(ultrasonic_sensor)
target_include_directories(ultrasonic_sensor PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# pull in common dependencies
target_link_libraries(ultrasonic_sensor pico_stdlib pico_stdio pico_time)