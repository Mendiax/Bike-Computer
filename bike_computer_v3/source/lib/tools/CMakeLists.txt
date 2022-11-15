aux_source_directory(./src DIR_Tools_SRCS)

include_directories(inc)
include_directories(include)

add_compile_options(-Wint-to-pointer-cast 
                    -Wl,-print-memory-usage 
                    -Wall -Wextra -Wstack-usage=128
                    )

add_library(Tools ${DIR_Tools_SRCS})
target_link_libraries(Tools PUBLIC pico_stdlib)