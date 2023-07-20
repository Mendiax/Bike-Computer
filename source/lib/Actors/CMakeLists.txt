aux_source_directory(./source DIR_Actors_SRCS)



add_library(Actors ${DIR_Actors_SRCS})
target_include_directories(Actors PUBLIC include)


set(required_libraries pico_stdlib Tools)
if (NOT BUILD_MOCK_MULTICORE)
else()
    list(APPEND required_libraries MOCK_MULTICORE MOCK_GPIO)
endif()

target_link_libraries(Actors PUBLIC ${required_libraries})