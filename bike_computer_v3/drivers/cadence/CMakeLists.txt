aux_source_directory(./src DIR_Cadence_SRCS)

include_directories(include)
include_directories(../interrupts/include)



add_library(Cadence ${DIR_Cadence_SRCS})
target_link_libraries(Cadence PUBLIC pico_stdlib Interrupts)
