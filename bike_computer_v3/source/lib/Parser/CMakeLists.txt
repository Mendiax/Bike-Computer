aux_source_directory(./src DIR_Parser_SRCS)

include_directories(include)

add_compile_options(-Wall -Wextra)

add_library(Parser src/lexer.cpp src/parser.cpp src/parser_bison.cpp)
target_link_libraries(Parser PUBLIC pico_stdlib)