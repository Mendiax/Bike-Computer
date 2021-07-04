aux_source_directory(./source DIR_Actors_SRCS)



add_library(Actors ${DIR_Actors_SRCS})
target_include_directories(Actors PUBLIC include)
target_link_libraries(Actors PUBLIC pico_stdlib Tools)