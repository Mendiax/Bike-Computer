aux_source_directory(./source DIR_Gui_SRCS)

# include_directories(common/include)
# include_directories(session/include)

include_directories(include)



# add_subdirectory(common)
# add_subdirectory(main_menu)
# add_subdirectory(session)

add_library(Gui ${DIR_Gui_SRCS})
target_link_libraries(Gui PUBLIC pico_stdlib Buttons Common Views)
