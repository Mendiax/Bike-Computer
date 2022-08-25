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


add_compile_options(-Wfatal-errors )

# add_compile_options(-Wint-to-pointer-cast 
#                     -Wl,-print-memory-usage 
#                     #-Wall 
#                     #-Wextra 
#                     -Wstack-usage=128
#                     -Wformat-overflow=2
#                     -Wformat=2
#                     -Wfatal-errors
#                     )


############################# INCLUDE DIRECTORIES #############################

include_directories(common/include)
include_directories(drivers/display/include)
include_directories(lib/tools/include)
include_directories(lib/views/include)
include_directories(drivers/speedometer/include)
include_directories(drivers/interrupts/include)
include_directories(lib/Console)
include_directories(drivers/buttons/include)
include_directories(drivers/battery/include)
include_directories(drivers/gsm/include)

# GSM
# include_directories(./lib2/Config)
# include_directories(./lib2/Infrared)
# include_directories(./lib2/AT_TEST)
# include_directories(./lib2/BT)
# include_directories(./lib2/GPS)
# include_directories(./lib2/HTTP)
# include_directories(./lib2/Phone_Call)
# include_directories(./lib2/SMS)
# 
# include_directories(./examples/)
# add_subdirectory(./examples)



#DOF
include_directories(./drivers/DOF/BMP280)
include_directories(./drivers/DOF/I2C)
include_directories(./drivers/DOF/IMU)
include_directories(./drivers/DOF/MPU9250)

#tests
include_directories(tests/bc_test)
include_directories(tests)

include_directories(lib/Parser/include)




################################ SUBDIRECTORY #################################
#sub



add_subdirectory(common)
add_subdirectory(drivers/display)
add_subdirectory(lib/tools)
add_subdirectory(lib/views)
add_subdirectory(drivers/speedometer)
add_subdirectory(lib/Console)
add_subdirectory(drivers/interrupts)
add_subdirectory(drivers/buttons)
add_subdirectory(drivers/battery)
add_subdirectory(drivers/gsm)
add_subdirectory(lib/Parser)


# DOF
add_subdirectory(drivers/DOF/BMP280)
add_subdirectory(drivers/DOF/I2C)
add_subdirectory(drivers/DOF/IMU)
add_subdirectory(drivers/DOF/MPU9250)

# add_subdirectory(lib_DOF/BMP280_pico)

# GSM
# add_subdirectory(lib2/Config)
# add_subdirectory(lib2/Infrared)
# add_subdirectory(lib2/AT_TEST)
# add_subdirectory(lib2/BT)
# add_subdirectory(lib2/GPS)
# add_subdirectory(lib2/HTTP)
# add_subdirectory(lib2/Phone_Call)
# add_subdirectory(lib2/SMS)

# SD
add_subdirectory(external/no-OS-FatFS/FatFs_SPI)



# include_directories(./lib_DOF/BMP280_pico)

# tests
add_subdirectory(tests/bc_test)
add_subdirectory(tests)




#################################### MAIN #####################################

message("PICO_MAIN_FILE=${PICO_MAIN_FILE}")
if( NOT PICO_MAIN_FILE ) 
        SET(PICO_MAIN_FILE pico_main)
        message("SET PICO_MAIN_FILE = ${PICO_MAIN_FILE}")
endif()
add_executable( ${PICO_MAIN_FILE} app/main.cpp)
target_compile_definitions(${PICO_MAIN_FILE} PRIVATE PICO_MALLOC_PANIC=0)

pico_enable_stdio_usb( ${PICO_MAIN_FILE} 1)
pico_enable_stdio_uart( ${PICO_MAIN_FILE} 0)

pico_add_extra_outputs( ${PICO_MAIN_FILE})

target_link_libraries( ${PICO_MAIN_FILE} 
        pico_stdlib
        hardware_spi
        hardware_adc
        pico_multicore

        Display
        Tools
        Speedometer
        Interrupts
        Views
        Buttons
        Common
        Battery
        GSM

        Parser
)

#################################### TEST #####################################
# TODO tests: https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake

message("PICO_TEST_FILE=${PICO_TEST_FILE}")
if( NOT PICO_TEST_FILE ) 
        SET(PICO_TEST_FILE pico_test)
        message("SET PICO_TEST_FILE = ${PICO_TEST_FILE}")
endif()
add_executable(${PICO_TEST_FILE} app/test.cpp)
#add_executable(${PICO_TEST_FILE} tests/test.cpp)

pico_enable_stdio_usb(${PICO_TEST_FILE} 1)
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
        Speedometer
        Console
        GSM

        # Infrared_GSM Config_GSM AT_TEST BT GPS HTTP Phone_Call SMS
        # examples

        IMU I2C
        FatFs_SPI

        Parser

        test_main # tests
)

#################################### Console #####################################
# TODO tests: https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake

message("PICO_GSM_CONSOLE_FILE=${PICO_GSM_CONSOLE_FILE}")
if( NOT PICO_GSM_CONSOLE_FILE ) 
        SET(PICO_GSM_CONSOLE_FILE pico_gsm_console)
        message("SET PICO_GSM_CONSOLE_FILE = ${PICO_GSM_CONSOLE_FILE}")
endif()
add_executable(${PICO_GSM_CONSOLE_FILE} app/gsm_console.cpp)

pico_enable_stdio_usb(${PICO_GSM_CONSOLE_FILE} 1)
pico_enable_stdio_uart(${PICO_GSM_CONSOLE_FILE} 0)

pico_add_extra_outputs(${PICO_GSM_CONSOLE_FILE})

target_link_libraries( ${PICO_GSM_CONSOLE_FILE}
        pico_stdlib
        pico_multicore
        hardware_spi
        hardware_adc
        hardware_uart

        Display
        Tools
        Speedometer
        Console

        GSM
        # Infrared_GSM Config_GSM AT_TEST BT GPS HTTP Phone_Call SMS
        # examples
)



