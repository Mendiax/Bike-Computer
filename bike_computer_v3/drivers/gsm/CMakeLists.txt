aux_source_directory(./src DIR_GSM_SRCS)

#include_directories(inc)
include_directories(include)
#include_directories(../tools/include)

add_library(GSM ${DIR_GSM_SRCS})
target_include_directories(GSM PUBLIC ./include)
target_link_libraries(GSM PUBLIC 
    pico_stdlib
    hardware_uart
    Console
)