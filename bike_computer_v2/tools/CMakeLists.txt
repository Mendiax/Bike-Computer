aux_source_directory(./src DIR_Tools_SRCS)

include_directories(inc)
include_directories(include)


add_library(Tools ${DIR_Tools_SRCS})
#target_link_libraries(Tools PUBLIC pico_stdlib)