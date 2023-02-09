aux_source_directory(. DIR_bcTest_SRCS)
include_directories(./)

add_library(bcTest ${DIR_bcTest_SRCS})
target_include_directories(bcTest PUBLIC include)
target_link_libraries(bcTest PUBLIC
    pico_time
    pico_stdlib
)
# if (NOT TARGET pico_stdlib_headers)
#     add_library(pico_stdlib_headers INTERFACE)
#     target_include_directories(pico_stdlib_headers INTERFACE include)
#     target_link_libraries(pico_stdlib_headers INTERFACE
#             hardware_gpio
#             hardware_uart
#             hardware_divider
#             pico_time
#             pico_util
#     )
# endif()