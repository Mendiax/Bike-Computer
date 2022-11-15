aux_source_directory(./source DIR_Gui_Impl_SRCS)

include_directories(include)

add_library(GUI_Impl ${DIR_Gui_Impl_SRCS})
target_link_libraries(GUI_Impl PUBLIC pico_stdlib
    Gui
    Common)