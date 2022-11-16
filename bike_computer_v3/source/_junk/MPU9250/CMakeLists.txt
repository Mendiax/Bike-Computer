aux_source_directory(. DIR_MPU9250_SRCS)
include_directories(../I2C)


add_library(MPU9250 ${DIR_MPU9250_SRCS})
target_link_libraries(MPU9250 PUBLIC I2C pico_stdlib)