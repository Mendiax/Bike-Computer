aux_source_directory(./src DIR_Tools_SRCS)

include_directories(../../drivers/display/include)

add_library(Views)
target_link_libraries(Views PUBLIC pico_stdlib Display)