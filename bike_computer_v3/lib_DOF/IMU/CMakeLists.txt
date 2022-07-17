aux_source_directory(. DIR_IMU_SRCS)
include_directories(../BMP280)
include_directories(../MPU9250)
include_directories(../../lib/Console)


add_library(IMU ${DIR_IMU_SRCS})
target_link_libraries(IMU PUBLIC BMP280 MPU9250 pico_stdlib Console)