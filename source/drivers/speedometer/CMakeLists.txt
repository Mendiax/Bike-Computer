aux_source_directory(./src DIR_Speedometer_SRCS)






add_library(Speedometer ${DIR_Speedometer_SRCS})
target_include_directories(Speedometer PUBLIC include)
target_link_libraries(Speedometer PUBLIC pico_stdlib Interrupts Common)

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