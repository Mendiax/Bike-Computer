aux_source_directory(./src DIR_Views_SRCS)
aux_source_directory(./src/screenfunc DIR_Views_screenfunc_SRCS)

include_directories(include)
include_directories(../../drivers/display/include)

add_library(Views ${DIR_Views_SRCS} ${DIR_Views_screenfunc_SRCS})
target_link_libraries(Views PUBLIC pico_stdlib Display Common)