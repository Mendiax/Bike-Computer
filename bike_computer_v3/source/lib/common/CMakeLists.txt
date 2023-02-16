aux_source_directory(./src DIR_Common_SRCS)
aux_source_directory(./src/gui DIR_Gui_SRCS)





add_library(Common ${DIR_Common_SRCS} ${DIR_Gui_SRCS})
target_include_directories(Common PUBLIC include)

target_link_libraries(Common PUBLIC
                      pico_multicore pico_stdlib
                      Tools
                      )
