aux_source_directory(. DIR_BMP_pico_SRCS)
#include_directories(../I2C)


add_library(BMP280_pico ${DIR_BMP_pico_SRCS})
target_link_libraries(BMP280_pico PUBLIC hardware_i2c pico_stdlib)