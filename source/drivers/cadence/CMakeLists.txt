aux_source_directory(./src DIR_Cadence_SRCS)


add_library(Cadence ${DIR_Cadence_SRCS})
target_include_directories(Cadence PUBLIC include)

set(required_libraries pico_stdlib Interrupts Filters)

# add hw libs only if needed
if(PICO_PLATFORM MATCHES "host")
    list(APPEND required_libraries MOCK_GPIO)
endif()

target_link_libraries(Cadence PUBLIC ${required_libraries})

