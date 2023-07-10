aux_source_directory(./src DIR_FatFs_SPI_SRCS)

add_library(FatFs_SPI ${DIR_FatFs_SPI_SRCS})
target_include_directories(FatFs_SPI PUBLIC include)
target_link_libraries(FatFs_SPI PUBLIC pico_stdlib Tools)