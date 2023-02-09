aux_source_directory(./source DIR_BMP280_SRCS)

include_directories(../I2C/include)
include_directories(include)

add_library(BMP280 ${DIR_BMP280_SRCS})
target_link_libraries(BMP280 PUBLIC hardware_i2c pico_stdlib I2C)