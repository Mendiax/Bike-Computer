aux_source_directory(./src DIR_GSM_SRCS)





add_library(GSM ${DIR_GSM_SRCS})
target_include_directories(GSM PUBLIC ./include)

set(required_libraries pico_stdlib Common)

# add hw libs only if needed
if(PICO_PLATFORM MATCHES "host")
    list(APPEND required_libraries MOCK_GPIO)
else()
    list(APPEND required_libraries hardware_uart)
endif()

target_link_libraries(GSM PUBLIC ${required_libraries})
