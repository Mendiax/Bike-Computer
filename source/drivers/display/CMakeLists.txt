aux_source_directory(./src DIR_display_SRCS)
aux_source_directory(./src_hw DIR_display_hw_SRCS)
aux_source_directory(./src_mock DIR_display_hw_mock_SRCS)

if (BUILD_MOCK_DISPLAY)
    set(DIR_display_sources ${DIR_display_SRCS} ${DIR_display_hw_mock_SRCS})
else()
    set(DIR_display_sources ${DIR_display_SRCS} ${DIR_display_hw_SRCS})
endif()


add_library(Display  ${DIR_display_sources})
target_include_directories(Display PUBLIC ./include)


set(required_libraries pico_stdlib Tools)

# add hw libs only if needed
if (NOT BUILD_MOCK_DISPLAY)
    list(APPEND required_libraries hardware_spi hardware_pwm)
else()
    list(APPEND required_libraries Tools IMGUI_SDL2 Speedometer Buttons)
endif()

target_link_libraries(Display PUBLIC ${required_libraries})




