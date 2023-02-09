aux_source_directory(./src DIR_Buttons_SRCS)

include_directories(include)
include_directories(../interrupts/include)



add_library(Buttons ${DIR_Buttons_SRCS})
target_link_libraries(Buttons PUBLIC Interrupts)
