aux_source_directory(./src DIR_Interrupt_SRCS)

include_directories(include)


add_library(Interrupts ${DIR_Interrupt_SRCS})
target_link_libraries(Interrupts PUBLIC pico_stdlib)
