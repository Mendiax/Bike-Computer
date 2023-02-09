aux_source_directory(./source DIR_Actors_SRCS)

include_directories(include)

add_library(Actors ${DIR_Actors_SRCS})
target_link_libraries(Actors PUBLIC pico_stdlib)