aux_source_directory(./src DIR_Speedometer_SRCS)

add_library(Speedometer ${DIR_Speedometer_SRCS})
target_include_directories(Speedometer PUBLIC include)

set(required_libraries pico_stdlib Interrupts Common)

# add hw libs only if needed
if(PICO_PLATFORM MATCHES "host")
    list(APPEND required_libraries MOCK_GPIO)
endif()

target_link_libraries(Speedometer PUBLIC ${required_libraries})