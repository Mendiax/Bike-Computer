aux_source_directory(./source DIR_Filters_SRCS)



add_library(Filters ${DIR_Filters_SRCS})
target_include_directories(Filters PUBLIC include)
target_link_libraries(Filters PUBLIC pico_stdlib Tools)