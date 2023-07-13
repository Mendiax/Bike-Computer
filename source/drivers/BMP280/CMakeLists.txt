aux_source_directory(./src DIR_BMP280_SRCS)
aux_source_directory(./src_hw DIR_BMP280_hw_SRCS)
aux_source_directory(./src_mock DIR_BMP280_mock_SRCS)

if (BUILD_MOCK_BMP280)
    set(DIR_bmp280_sources ${DIR_BMP280_SRCS} ${DIR_BMP280_mock_SRCS})
else()
    set(DIR_bmp280_sources ${DIR_BMP280_SRCS} ${DIR_BMP280_hw_SRCS})
endif()



add_library(BMP280 ${DIR_bmp280_sources})
target_include_directories(BMP280 PUBLIC include)

set(required_libraries pico_stdlib Tools Common Filters)

# add hw libs only if needed
if (NOT BUILD_MOCK_BMP280)
    list(APPEND required_libraries I2C)
endif()

target_link_libraries(BMP280 PUBLIC ${required_libraries})
