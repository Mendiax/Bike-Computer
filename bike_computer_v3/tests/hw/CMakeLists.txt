# aux_source_directory(. DIR_test_main_SRCS)
# include_directories(./)
# include_directories(../../bc_test)

# add_library(test_main ${DIR_test_main_SRCS})
# target_include_directories(test_main PUBLIC .)
# target_link_libraries(test_main PUBLIC
#     pico_time
#     pico_stdlib
#     GSM
#     Parser
#     Common
#     bcTest
#     Display
#     BMP280
#     SD
#     Actors
#     # EPROM
# )
if(PICO_PLATFORM MATCHES "rp2040")
message("[INFO] [CMAKE] generate target tests")

message("[INFO] [CMAKE] PICO_TEST_FILE=${PICO_TEST_FILE}")
if( NOT PICO_TEST_FILE )
        SET(PICO_TEST_FILE target_test)
        message("[INFO] [CMAKE] SET PICO_TEST_FILE = ${PICO_TEST_FILE}")
endif()


# pico tests paths
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
# set(PICO_TEST_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../external/pico_tests")






# message("PICO_TEST_PATH=${PICO_TEST_PATH}")
# message("PICO_TEST_RUN_PATH=${PICO_TEST_RUN_PATH}")


# include_directories(./)
add_executable(${PICO_TEST_FILE} target_main.cpp)
target_compile_definitions(${PICO_TEST_FILE} PRIVATE PICO_MALLOC_PANIC=0)


    # gen header
        # set(PICO_TEST_SRC_PATH "${PICO_TEST_PATH}/pico_tests")
        # set(PICO_TEST_RUN_PATH "${PICO_TEST_PATH}/run")
        # # Define the path to the Python script
        # set(PYTHON_SCRIPT_FILE "${PICO_TEST_SRC_PATH}/scripts/generate_header.py")

        # # Define the source and header directories
        set(HEADER_DIR ${SOURCE_DIR})

        # # Define the full path to the header file
        set(HEADER_FILE "${HEADER_DIR}/test_cases.h")

        # # Define the command to generate the header file
        # add_custom_command(
        # OUTPUT ${HEADER_FILE}
        # COMMAND python3 -u ${PYTHON_SCRIPT_FILE} ${HEADER_FILE} ${SOURCE_DIR}
        # DEPENDS ${PYTHON_SCRIPT_FILE} # List any dependencies of the command here
        # COMMENT "Generating header file"
        # )
        # Add the header file as a target
        # add_custom_target(
        # generate_header_file ALL
        # DEPENDS ${HEADER_FILE}
        # )
        # Add the header file as an include directory for the target
        include_directories(${HEADER_DIR})
        # Depend the target on the header file generation target
        # add_dependencies(${PICO_TEST_FILE} generate_header_file)

        # add sources
        aux_source_directory(${SOURCE_DIR} DIR_src_SRCS)
        target_sources(${PICO_TEST_FILE} PRIVATE  ${DIR_src_SRCS})












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
    add_test( NAME target_test COMMAND ${PYTHON_EXECUTABLE} -u ${PICO_TEST_PATH}/run/run.py ${PICO_TEST_RUN_CONFIG_LOCAL_PATH})
else()
    add_test(NAME target_test
             COMMAND ${PYTHON_EXECUTABLE} -u ${PICO_TEST_PATH}/run/run.py ${PICO_TEST_RUN_CONFIG_SSH_PATH}
            #  WORKING_DIRECTORY ${PICO_TEST_RUN_PATH}
             )
endif()
#  WORKING_DIRECTORY ${PICO_TEST_RUN_PATH}


endif()