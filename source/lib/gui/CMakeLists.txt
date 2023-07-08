aux_source_directory(./source DIR_Gui_SRCS)

add_library(Gui ${DIR_Gui_SRCS})
target_include_directories(Gui PUBLIC include)

target_link_libraries(Gui PUBLIC pico_stdlib Buttons Common Views)
