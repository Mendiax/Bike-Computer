aux_source_directory(src/ DIR_SD_SRCS)



add_library(SD ${DIR_SD_SRCS})
target_include_directories(SD PUBLIC include)

target_link_libraries(SD PUBLIC pico_stdlib FatFs_SPI Common)