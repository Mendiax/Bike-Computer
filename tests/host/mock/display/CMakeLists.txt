aux_source_directory(./src DIR_MOCK_DISPLAY_SRCS)

add_library(MOCK_DISPLAY ${DIR_MOCK_DISPLAY_SRCS})
target_include_directories(MOCK_DISPLAY PUBLIC include)
target_link_libraries(MOCK_DISPLAY PUBLIC pico_stdlib Tools IMGUI_SDL2
    Display
    Speedometer)