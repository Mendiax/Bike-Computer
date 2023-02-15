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

message("generate target tests")

message("PICO_TEST_FILE=${PICO_TEST_FILE}")
if( NOT PICO_TEST_FILE )
        SET(PICO_TEST_FILE pico_test)
        message("SET PICO_TEST_FILE = ${PICO_TEST_FILE}")
endif()
include_directories(./)
add_executable(${PICO_TEST_FILE} target_main.cpp ${DIR_test_main_SRCS})
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
    Actors)

pico_enable_stdio_usb(${PICO_TEST_FILE} ENABLED)
pico_enable_stdio_uart(${PICO_TEST_FILE} 0)
pico_add_extra_outputs(${PICO_TEST_FILE})