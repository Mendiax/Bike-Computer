aux_source_directory(./src DIR_RTC_SRCS)

add_library(MOCK_SIM868 ${DIR_RTC_SRCS})
target_include_directories(MOCK_SIM868 PUBLIC include)
target_link_libraries(MOCK_SIM868 PUBLIC pico_stdlib Tools MOCK_GPIO)