aux_source_directory(./src DIR_MPU9250_SRCS)

add_library(MPU9250 ${DIR_MPU9250_SRCS})
target_include_directories(MPU9250 PUBLIC include)

set(required_libraries pico_stdlib Interrupts Common)

# add hw libs only if needed
if (NOT BUILD_MOCK_MPU)
    list(APPEND required_libraries I2C)
endif()

target_link_libraries(MPU9250 PUBLIC ${required_libraries})