aux_source_directory(./src DIR_Battery_SRCS)

include_directories(include)

add_library(Battery ${DIR_Battery_SRCS})
target_link_libraries(Battery PUBLIC pico_stdlib hardware_adc)
