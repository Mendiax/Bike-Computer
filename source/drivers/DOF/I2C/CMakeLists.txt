aux_source_directory(. DIR_I2C_SRCS)

add_library(I2C ${DIR_I2C_SRCS})
target_link_libraries(I2C PUBLIC hardware_i2c pico_stdlib pico_stdlib)