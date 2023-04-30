if(PICO_PLATFORM MATCHES "rp2040")
message("[INFO] [CMAKE] generate target tests")

message("[INFO] [CMAKE] PICO_TEST_FILE=${PICO_TEST_FILE}")
if( NOT PICO_TEST_FILE )
        SET(PICO_TEST_FILE target_test)
        message("[INFO] [CMAKE] SET PICO_TEST_FILE = ${PICO_TEST_FILE}")
endif()


set(HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
file(GLOB_RECURSE HW_TEST_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

add_executable(${PICO_TEST_FILE} target_main.cpp)
target_compile_definitions(${PICO_TEST_FILE} PRIVATE PICO_MALLOC_PANIC=0)

include_directories(${HEADER_DIR})
target_sources(${PICO_TEST_FILE} PRIVATE  ${HW_TEST_SRC_FILES} ${SD_CONFIG_SRC})


target_link_libraries(${PICO_TEST_FILE}
    pico_time
    pico_stdlib
    GSM
    Parser
    Common
    bcTest
    Display
    BMP280
    SD
    Actors
    pico_test
    Interrupts
    Speedometer
    Cadence
    )

pico_enable_stdio_usb(${PICO_TEST_FILE} ENABLED)
pico_enable_stdio_uart(${PICO_TEST_FILE} 0)
pico_add_extra_outputs(${PICO_TEST_FILE})

# locals tests

set(PICO_TEST_RUN_CONFIG_LOCAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/hw_config/config_local.yaml")
set(PICO_TEST_RUN_CONFIG_SSH_PATH "${CMAKE_CURRENT_SOURCE_DIR}/hw_config/config.yaml")

# TODO add swithc
include(FindPythonInterp)
# set(PICO_TEST_HOST "ssh")
message("[INFO] [CMAKE] PICO_TEST_HOST = ${PICO_TEST_HOST}")
if(PICO_TEST_HOST MATCHES "local")
    add_test( NAME target_test COMMAND python3 -u ${PICO_TEST_PATH}/run/run.py ${PICO_TEST_RUN_CONFIG_LOCAL_PATH})
else()
    add_test(NAME target_test
             COMMAND python3 -u ${PICO_TEST_PATH}/run/run.py ${PICO_TEST_RUN_CONFIG_SSH_PATH}
            #  WORKING_DIRECTORY ${PICO_TEST_RUN_PATH}
             )
endif()

endif()