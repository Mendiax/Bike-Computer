aux_source_directory(./source DIR_EPROM_SRCS)
include_directories(include)


add_library(EPROM ${DIR_EPROM_SRCS})
target_link_libraries(EPROM PUBLIC I2C pico_stdlib)