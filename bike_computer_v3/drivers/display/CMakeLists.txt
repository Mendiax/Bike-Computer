aux_source_directory(./src DIR_Oled_SRCS)

#include_directories(inc)
include_directories(include)
#include_directories(../tools/include)

add_library(Display ${DIR_Oled_SRCS})
target_include_directories(Display PUBLIC ./include)
target_link_libraries(Display PUBLIC 
    pico_stdlib
    hardware_spi
    hardware_pwm
)