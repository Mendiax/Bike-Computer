aux_source_directory(./source DIR_Cores_SRCS)






add_library(Cores ${DIR_Cores_SRCS})

target_include_directories(Cores PUBLIC include)

set(required_libraries
    pico_stdlib
    GUI_Impl
    Display
    Speedometer
    Cadence
    MPU9250
    DOF
    Common
    GSM
    Tools
    Actors
    SD
    )

if(PICO_PLATFORM MATCHES "host")
    list(APPEND required_libraries RTC)
else()
    # list(APPEND required_libraries hardware_uart)
endif()
target_link_libraries(Cores PUBLIC ${required_libraries})


