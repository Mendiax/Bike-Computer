aux_source_directory(./source DIR_Gui_Impl_SRCS)



add_library(GUI_Impl ${DIR_Gui_Impl_SRCS})
target_include_directories(GUI_Impl PUBLIC include)
target_link_libraries(GUI_Impl PUBLIC pico_stdlib
    Gui
    Common
    Cores)



#     if (NOT TARGET GUI_Impl)
#     aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/source DIR_Gui_Impl_SRCS)

#     add_library(GUI_Impl ${DIR_Gui_Impl_SRCS})

#     # target_sources(GUI_Impl INTERFACE ${DIR_Gui_Impl_SRCS})

#     target_include_directories(GUI_Impl PUBLIC include)

#     target_link_libraries(GUI_Impl PUBLIC pico_stdlib
#         Gui
#         Common)

# endif()