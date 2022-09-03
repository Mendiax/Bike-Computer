aux_source_directory(. DIR_test_main_SRCS)
include_directories(./)

add_library(test_main ${DIR_test_main_SRCS})
target_link_libraries(test_main PUBLIC
    pico_time
    pico_stdlib
    GSM
    Parser
    Common
    bcTest
    Display
    BMP280
    SD
)