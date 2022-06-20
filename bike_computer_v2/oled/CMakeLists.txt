aux_source_directory(./src DIR_Oled_SRCS)
aux_source_directory(./src/views DIR_Oled_views_SRCS)

include_directories(inc)
include_directories(include)
include_directories(../tools/include)


add_library(Oled ${DIR_Oled_SRCS} ${DIR_Oled_views_SRCS})
target_link_libraries(Oled PUBLIC pico_stdlib hardware_spi Tools)