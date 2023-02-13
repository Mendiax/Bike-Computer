aux_source_directory(./src DIR_Buttons_SRCS)






add_library(Buttons ${DIR_Buttons_SRCS})
target_include_directories(Buttons PUBLIC include)

target_link_libraries(Buttons PUBLIC Interrupts)
