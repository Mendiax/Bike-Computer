cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(pico
        VERSION 3.0
        LANGUAGES CXX ASM C)

pico_sdk_init()

if( NOT CMAKE_BUILD_TYPE )
        SET(CMAKE_BUILD_TYPE Debug)
        message("BUILD TYPE NOT SPECIFIED set to ${CMAKE_BUILD_TYPE} on default")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(CMAKE_C_COMPILE_OPTIONS_COLOR_DIAGNOSTICS     -DCOLOR_ON)

add_compile_options(-Wfatal-errors -Werror=return-type ) # -Wall -Wextra

# add_compile_options(-Wint-to-pointer-cast
#                     -Wl,-print-memory-usage
#                     #-Wall
#                     #-Wextra
#                     -Wstack-usage=128
#                     -Wformat-overflow=2
#                     -Wformat=2
#                     -Wfatal-errors
#                     )

#-----------------------------------------------------------------------------#
#                             INCLUDE DIRECTORIES                             #
#-----------------------------------------------------------------------------#



include_directories(source/lib/common/include)
include_directories(source/lib/tools/include)
include_directories(source/lib/views/include)
include_directories(source/lib/SD/include)
include_directories(source/lib/Parser/include)
include_directories(source/lib/Actors/include)
include_directories(source/lib/gui/include)

include_directories(source/drivers/display/include)
include_directories(source/drivers/speedometer/include)
include_directories(source/drivers/cadence/include)
include_directories(source/drivers/interrupts/include)
include_directories(source/drivers/buttons/include)
include_directories(source/drivers/sim868/include)
# include_directories(source/drivers/EPROM/include)

#DOF
include_directories(source/drivers/BMP280/include)
include_directories(source/drivers/I2C/include)
# include_directories(source/drivers/DOF/IMU)
# include_directories(source/drivers/DOF/MPU9250)

include_directories(source/app/gui/include)
include_directories(source/app/cores/include)

#tests
include_directories(source/tests/bc_test)
include_directories(source/tests/tc)





################################ SUBDIRECTORY #################################



add_subdirectory(source/lib/common)
add_subdirectory(source/lib/tools)
add_subdirectory(source/lib/views)
add_subdirectory(source/lib/Parser)
add_subdirectory(source/lib/SD)
add_subdirectory(source/lib/Actors)
add_subdirectory(source/lib/gui)


add_subdirectory(source/drivers/display)
add_subdirectory(source/drivers/speedometer)
add_subdirectory(source/drivers/cadence)
add_subdirectory(source/drivers/interrupts)
add_subdirectory(source/drivers/buttons)
add_subdirectory(source/drivers/sim868)
# add_subdirectory(source/drivers/EPROM)

# DOF
add_subdirectory(source/drivers/BMP280)
add_subdirectory(source/drivers/I2C)
# add_subdirectory(source/drivers/DOF/IMU)
# add_subdirectory(source/drivers/DOF/MPU9250)

# SD
add_subdirectory(source/external/no-OS-FatFS/FatFs_SPI)

add_subdirectory(source/app/gui)
add_subdirectory(source/app/cores)

# tests
add_subdirectory(source/tests/bc_test)
add_subdirectory(source/tests/tc)




#################################### MAIN #####################################

message("PICO_MAIN_FILE=${PICO_MAIN_FILE}")
if( NOT PICO_MAIN_FILE )
        SET(PICO_MAIN_FILE pico_main)
        message("SET PICO_MAIN_FILE = ${PICO_MAIN_FILE}")
endif()
add_executable( ${PICO_MAIN_FILE} source/app/main.cpp)
target_compile_definitions(${PICO_MAIN_FILE} PRIVATE PICO_MALLOC_PANIC=0)

pico_enable_stdio_usb( ${PICO_MAIN_FILE} 1)
pico_enable_stdio_uart( ${PICO_MAIN_FILE} 0)

pico_add_extra_outputs( ${PICO_MAIN_FILE})

target_link_libraries( ${PICO_MAIN_FILE}
        pico_stdlib
        pico_multicore
        Tools
        Cores
)

#################################### TEST #####################################
# TODO tests: https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake

message("PICO_TEST_FILE=${PICO_TEST_FILE}")
if( NOT PICO_TEST_FILE )
        SET(PICO_TEST_FILE pico_test)
        message("SET PICO_TEST_FILE = ${PICO_TEST_FILE}")
endif()
# add_executable(${PICO_TEST_FILE} source/app/test.cpp)
add_executable(${PICO_TEST_FILE} source/tests/tc_basic.cpp)

pico_enable_stdio_usb(${PICO_TEST_FILE} ENABLED)
pico_enable_stdio_uart(${PICO_TEST_FILE} 0)

pico_add_extra_outputs(${PICO_TEST_FILE})

target_link_libraries( ${PICO_TEST_FILE}
        pico_stdlib
        pico_multicore
        hardware_spi
        hardware_adc
        hardware_uart

        Display
        Tools
        Cadence
        Speedometer

        GSM

        # IMU I2C
        # FatFs_SPI

        Actors
        # EPROM
        test_main
        Parser

)

#################################### Console #####################################
# TODO tests: https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake

message("PICO_GSM_CONSOLE_FILE=${PICO_GSM_CONSOLE_FILE}")
if( NOT PICO_GSM_CONSOLE_FILE )
        SET(PICO_GSM_CONSOLE_FILE pico_gsm_console)
        message("SET PICO_GSM_CONSOLE_FILE = ${PICO_GSM_CONSOLE_FILE}")
endif()
add_executable(${PICO_GSM_CONSOLE_FILE} source/app/gsm_console.cpp)

pico_enable_stdio_usb(${PICO_GSM_CONSOLE_FILE} 1)
pico_enable_stdio_uart(${PICO_GSM_CONSOLE_FILE} 0)

pico_add_extra_outputs(${PICO_GSM_CONSOLE_FILE})

target_link_libraries( ${PICO_GSM_CONSOLE_FILE}
        # pico_stdlib
        # pico_multicore
        # hardware_spi
        # hardware_adc
        # hardware_uart

        # Display
        # Tools
        # Cadence
        # Speedometer

        # Common

        # GSM
        pico_stdlib
        pico_multicore
        Tools
        Cores
)



