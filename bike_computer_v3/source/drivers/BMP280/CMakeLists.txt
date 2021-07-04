aux_source_directory(./source DIR_BMP280_SRCS)






add_library(BMP280 ${DIR_BMP280_SRCS})
target_include_directories(BMP280 PUBLIC include)
target_link_libraries(BMP280 PUBLIC hardware_i2c pico_stdlib I2C Common)