aux_source_directory(./src DIR_Interrupt_SRCS)




add_library(Interrupts ${DIR_Interrupt_SRCS})
target_include_directories(Interrupts PUBLIC include)

target_link_libraries(Interrupts PUBLIC pico_stdlib Common Cadence Buttons)
