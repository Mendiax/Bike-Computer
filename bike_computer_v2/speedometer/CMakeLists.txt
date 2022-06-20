aux_source_directory(./src DIR_Speedometer_SRCS)

include_directories(inc)
include_directories(include)


add_library(Speedometer ${DIR_Speedometer_SRCS})
target_link_libraries(Speedometer PUBLIC pico_stdlib)