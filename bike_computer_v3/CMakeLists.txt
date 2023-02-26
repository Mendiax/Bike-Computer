cmake_minimum_required(VERSION 3.17)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(pico
        VERSION 3.1
        LANGUAGES CXX ASM C)

pico_sdk_init()

if( NOT CMAKE_BUILD_TYPE )
        SET(CMAKE_BUILD_TYPE Debug)
        message("BUILD TYPE NOT SPECIFIED set to ${CMAKE_BUILD_TYPE} on default")
endif()


message("[INFO] [CMAKE] PICO_PLATFORM=${PICO_PLATFORM}")
if(PICO_PLATFORM MATCHES "rp2040")
    find_program(CXX_COMPILER NAMES arm-none-eabi-g++
      PATHS /usr/bin /usr/local/bin
      DOC "C++ compiler for target platform"
    )
    # Use the first compiler with -none-eabi that was found
    if (CXX_COMPILER)
      set(CMAKE_CXX_COMPILER ${CXX_COMPILER})
    endif()
endif()



# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set (CMAKE_CXX_STANDARD 17)

set(CMAKE_C_COMPILE_OPTIONS_COLOR_DIAGNOSTICS     -DCOLOR_ON)

message("[INFO] [CMAKE] C++ compiler: ${CMAKE_CXX_COMPILER}")
message("[INFO] [CMAKE] C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
message("[INFO] [CMAKE] C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

message("[INFO] [CMAKE] C compiler: ${CMAKE_C_COMPILER}")
message("[INFO] [CMAKE] C Compiler: ${CMAKE_C_COMPILER_ID}")
message("[INFO] [CMAKE] C Compiler Version: ${CMAKE_C_COMPILER_VERSION}")

# add_compile_options(-Wint-to-pointer-cast
#                     -Wl,-print-memory-usage
#                     #-Wall
#                     #-Wextra
#                     -Wstack-usage=128
#                     -Wformat-overflow=2
#                     -Wformat=2
#                     -Wfatal-errors
#                     )

add_subdirectory(source/lib/common)
add_subdirectory(source/lib/tools)
add_subdirectory(source/lib/Parser)
add_subdirectory(source/lib/Actors)

if(PICO_PLATFORM MATCHES "rp2040")
    add_compile_options(-Wfatal-errors -Werror=return-type -Wall -Wextra -Wformat-truncation)
    ################################ SUBDIRECTORY #################################

    add_subdirectory(source/lib/SD)
    add_subdirectory(source/lib/gui)
    add_subdirectory(source/lib/views)


    add_subdirectory(source/drivers/display)
    add_subdirectory(source/drivers/interrupts)
    add_subdirectory(source/drivers/speedometer)
    add_subdirectory(source/drivers/cadence)
    add_subdirectory(source/drivers/buttons)
    add_subdirectory(source/drivers/sim868)
    # add_subdirectory(source/drivers/EPROM)

    # DOF
    add_subdirectory(source/drivers/BMP280)
    add_subdirectory(source/drivers/I2C)
    # add_subdirectory(source/drivers/DOF/IMU)
    # add_subdirectory(source/drivers/DOF/MPU9250)

    # SD

    add_subdirectory(source/app/gui)
    add_subdirectory(source/app/cores)


    add_subdirectory(source/external/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)
    set(SD_CONFIG_SRC "${CMAKE_CURRENT_SOURCE_DIR}/source/external/FatFs_config/hw_config.c")


    #################################### MAIN #####################################

    message("PICO_MAIN_FILE=${PICO_MAIN_FILE}")
    if( NOT PICO_MAIN_FILE )
            SET(PICO_MAIN_FILE pico_main)
            message("SET PICO_MAIN_FILE = ${PICO_MAIN_FILE}")
    endif()
    add_executable( ${PICO_MAIN_FILE} source/app/main.cpp ${SD_CONFIG_SRC})
    target_compile_definitions(${PICO_MAIN_FILE} PRIVATE PICO_MALLOC_PANIC=0)

    pico_enable_stdio_usb( ${PICO_MAIN_FILE} 1)
    pico_enable_stdio_uart( ${PICO_MAIN_FILE} 0)

    pico_add_extra_outputs( ${PICO_MAIN_FILE})

    target_link_libraries( ${PICO_MAIN_FILE}
            pico_stdlib
            pico_multicore
            Tools
            Cores
            FatFs_SPI
    )

    # #################################### TEST #####################################
    # # TODO tests: https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake

    # message("PICO_TEST_FILE=${PICO_TEST_FILE}")
    # if( NOT PICO_TEST_FILE )
    #         SET(PICO_TEST_FILE pico_test)
    #         message("SET PICO_TEST_FILE = ${PICO_TEST_FILE}")
    # endif()
    # add_executable(${PICO_TEST_FILE} source/app/test.cpp  ${SD_CONFIG_SRC})
    # # add_executable(${PICO_TEST_FILE} source/tests/tc_basic.cpp ${SD_CONFIG_SRC})

    # pico_enable_stdio_usb(${PICO_TEST_FILE} ENABLED)
    # pico_enable_stdio_uart(${PICO_TEST_FILE} 0)

    # pico_add_extra_outputs(${PICO_TEST_FILE})

    # target_link_libraries( ${PICO_TEST_FILE}
    #         pico_stdlib
    #         pico_multicore
    #         hardware_spi
    #         hardware_adc
    #         hardware_uart

    #         Display
    #         Tools
    #         Cadence
    #         Speedometer

    #         GSM

    #         # IMU I2C

    #         Actors
    #         # EPROM
    #         test_main
    #         Parser

    # )

    #################################### Console #####################################
    # TODO tests: https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake

    message("PICO_GSM_CONSOLE_FILE=${PICO_GSM_CONSOLE_FILE}")
    if( NOT PICO_GSM_CONSOLE_FILE )
            SET(PICO_GSM_CONSOLE_FILE pico_gsm_console)
            message("SET PICO_GSM_CONSOLE_FILE = ${PICO_GSM_CONSOLE_FILE}")
    endif()
    add_executable(${PICO_GSM_CONSOLE_FILE} source/app/gsm_console.cpp ${SD_CONFIG_SRC})

    pico_enable_stdio_usb(${PICO_GSM_CONSOLE_FILE} 1)
    pico_enable_stdio_uart(${PICO_GSM_CONSOLE_FILE} 0)

    pico_add_extra_outputs(${PICO_GSM_CONSOLE_FILE})

    target_link_libraries( ${PICO_GSM_CONSOLE_FILE}
            pico_stdlib
            pico_multicore
            Tools
            Cores
    )

else()
# add_compile_options(-Wfatal-errors -Werror=return-type -Wall -Wextra)

    add_compile_options(-Wno-unused-variable -Werror=return-type)
endif()

# enable testing and define tests
enable_testing()
add_subdirectory(tests)



