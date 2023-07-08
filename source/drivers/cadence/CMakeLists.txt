aux_source_directory(./src DIR_Cadence_SRCS)






add_library(Cadence ${DIR_Cadence_SRCS})
target_include_directories(Cadence PUBLIC include)

target_link_libraries(Cadence PUBLIC pico_stdlib Interrupts)
