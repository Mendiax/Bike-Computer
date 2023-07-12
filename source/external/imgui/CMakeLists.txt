cmake_minimum_required(VERSION 3.13)

if (PICO_PLATFORM STREQUAL "host")
#     list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

    # need SDL2
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
#     find_package(SDL2_image REQUIRED)

    set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui)
    set(DIR_IMGUI_SDL2_SOURCES
        ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        # backend
        ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl2.cpp)

    add_library(IMGUI_SDL2 ${DIR_IMGUI_SDL2_SOURCES})


    target_include_directories(IMGUI_SDL2 PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${SDL2_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        )

    target_link_libraries(IMGUI_SDL2 PUBLIC ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})

    message("SDL loaded ${IMGUI_DIR} ${IMGUI_DIR}/backends ${SDL2_INCLUDE_DIRS}")


endif()

