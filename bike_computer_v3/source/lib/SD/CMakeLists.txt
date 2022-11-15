aux_source_directory(src/ DIR_SD_SRCS)

include_directories(include)

add_library(SD ${DIR_SD_SRCS})
target_link_libraries(SD PUBLIC pico_stdlib FatFs_SPI)