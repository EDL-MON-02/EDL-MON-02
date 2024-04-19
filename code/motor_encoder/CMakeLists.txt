cmake_minimum_required(VERSION 3.13)

# Initialize the SDK
include(pico_sdk_import.cmake )

project(motor_encoder C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(motor_encoder
        main.c
        )

# Enable USB output 
pico_enable_stdio_usb(motor_encoder 1)

# Enable UART output 
pico_enable_stdio_uart(motor_encoder 1)


# create map/bin/hex file etc.
pico_add_extra_outputs(motor_encoder)
target_include_directories(motor_encoder PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# pull in common dependencies
target_link_libraries(motor_encoder pico_stdlib pico_stdio pico_time hardware_pwm)