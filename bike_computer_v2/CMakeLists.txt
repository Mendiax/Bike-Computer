cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(pico)
pico_sdk_init()

add_compile_options(-Wint-to-pointer-cast)
#sub
add_subdirectory(oled)
add_subdirectory(tools)
add_subdirectory(speedometer)

#include directories
include_directories(./oled/include)
include_directories(./speedometer/include)
include_directories(./tools/include)



### MAIN ###
message("PICO_MAIN_FILE=${PICO_MAIN_FILE}")
if( NOT PICO_MAIN_FILE ) 
        SET(PICO_MAIN_FILE pico_main)
        message("SET PICO_MAIN_FILE = ${PICO_MAIN_FILE}")
endif()
add_executable( ${PICO_MAIN_FILE} main.cpp)

pico_enable_stdio_usb( ${PICO_MAIN_FILE} 1)
pico_enable_stdio_uart( ${PICO_MAIN_FILE} 0)

pico_add_extra_outputs( ${PICO_MAIN_FILE})

target_link_libraries( ${PICO_MAIN_FILE} 
        pico_stdlib
        hardware_spi
        Oled
        Tools
        Speedometer
        hardware_adc
)

### TEST ###
message("PICO_TEST_FILE=${PICO_TEST_FILE}")
if( NOT PICO_TEST_FILE ) 
        SET(PICO_TEST_FILE pico_test)
        message("SET PICO_TEST_FILE = ${PICO_TEST_FILE}")
endif()
add_executable(${PICO_TEST_FILE} test.cpp)

pico_enable_stdio_usb(${PICO_TEST_FILE} 1)
pico_enable_stdio_uart(${PICO_TEST_FILE} 0)

pico_add_extra_outputs(${PICO_TEST_FILE})

target_link_libraries( ${PICO_TEST_FILE} 
        pico_stdlib
        hardware_spi
        Oled
        Tools
        Speedometer
        hardware_adc
)