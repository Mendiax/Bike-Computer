aux_source_directory(./src DIR_Common_SRCS)

include_directories(include)


add_library(Common ${DIR_Common_SRCS})
target_link_libraries(Common PUBLIC
                      pico_multicore pico_stdlib
                      Tools Display Buttons Display Views
                      IMU I2C
                      SD
                      Cadence Speedometer
                      Actors
                      )
