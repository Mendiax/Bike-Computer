aux_source_directory(./src DIR_Parser_SRCS)

add_library(Parser src/lexer.cpp src/parser.cpp src/parser_bison.cpp)
target_include_directories(Parser PUBLIC include)

target_link_libraries(Parser PUBLIC pico_stdlib Common)