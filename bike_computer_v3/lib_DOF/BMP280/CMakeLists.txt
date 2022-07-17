aux_source_directory(. DIR_BMP_SRCS)
include_directories(../I2C)


add_library(BMP280 ${DIR_BMP_SRCS})
target_link_libraries(BMP280 PUBLIC I2C pico_stdlib)