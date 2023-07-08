# aux_source_directory(./src DIR_Tools_SRCS)


# add_library(Tools ${DIR_Tools_SRCS})
# target_link_libraries(Tools PUBLIC pico_stdlib)

if (NOT TARGET Tools)
    aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src DIR_Tools_SRCS)


    add_library(Tools ${DIR_Tools_SRCS})

    # target_sources(Tools INTERFACE ${DIR_Tools_SRCS})

    target_include_directories(Tools PUBLIC include)

    target_link_libraries(Tools PUBLIC pico_stdlib)

endif()