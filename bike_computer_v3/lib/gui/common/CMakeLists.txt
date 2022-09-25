aux_source_directory(./source DIR_gui_common_SRCS)

include_directories(include)


add_library(gui_common ${DIR_gui_common_SRCS})
target_link_libraries(gui_common PUBLIC
                      pico_multicore pico_stdlib
                      Tools Buttons Views
                      )
