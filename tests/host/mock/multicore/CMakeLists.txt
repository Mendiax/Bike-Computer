aux_source_directory(./src DIR_MOCK_MULTICORE_SRCS)

add_library(MOCK_MULTICORE ${DIR_MOCK_MULTICORE_SRCS})
target_include_directories(MOCK_MULTICORE PUBLIC include)
target_link_libraries(MOCK_MULTICORE PUBLIC pico_stdlib Tools)