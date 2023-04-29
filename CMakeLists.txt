cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(uart_echo)
pico_sdk_init()

add_executable(uart_echo main.c) 

target_include_directories(uart_echo PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(uart_echo pico_stdlib pico_stdio_usb pico_multicore)

pico_add_extra_outputs(uart_echo)
