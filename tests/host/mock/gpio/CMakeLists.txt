aux_source_directory(./src DIR_MOCK_GPIO_SRCS)

add_library(MOCK_GPIO ${DIR_MOCK_GPIO_SRCS})
target_include_directories(MOCK_GPIO PUBLIC include)
target_link_libraries(MOCK_GPIO PUBLIC pico_stdlib Tools)