aux_source_directory(./src DIR_RTC_SRCS)

add_library(RTC ${DIR_RTC_SRCS})
target_include_directories(RTC PUBLIC include)
target_link_libraries(RTC PUBLIC pico_stdlib Tools)