aux_source_directory(./src DIR_Views_SRCS)
aux_source_directory(./src/screenfunc DIR_Views_screenfunc_SRCS)

add_library(Views ${DIR_Views_SRCS} ${DIR_Views_screenfunc_SRCS})
target_include_directories(Views PUBLIC include)

target_link_libraries(Views PUBLIC pico_stdlib Display Common)