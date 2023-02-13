aux_source_directory(. DIR_BMP280_SRCS)



add_library(BMP280 ${DIR_BMP280_SRCS})
target_link_libraries(BMP280 PUBLIC hardware_i2c pico_stdlib)