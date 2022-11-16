aux_source_directory(./source DIR_Cores_SRCS)

include_directories(include)
include_directories(../gui/include)


add_library(Cores ${DIR_Cores_SRCS})
target_link_libraries(Cores PUBLIC pico_stdlib
    GUI_Impl
    Display
    Speedometer
    Cadence

    BMP280

    Common
    GSM
    Parser
    Tools)

    # Interrupts
    # Views
    # Buttons
