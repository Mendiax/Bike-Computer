aux_source_directory(./source DIR_Bat_SRCS)

add_library(Bat ${DIR_Bat_SRCS})
target_include_directories(Bat PUBLIC include)

target_link_libraries(Bat PUBLIC pico_stdlib Common)
