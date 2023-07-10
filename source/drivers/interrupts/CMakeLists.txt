aux_source_directory(./src DIR_Interrupts_SRCS)
aux_source_directory(./src_hw DIR_Interrupts_hw_SRCS)
aux_source_directory(./src_mock DIR_Interrupts_mock_SRCS)

if (BUILD_MOCK_INTERRUPTS)
    set(DIR_Interrupts_sources ${DIR_Interrupts_SRCS} ${DIR_Interrupts_mock_SRCS})
else()
    set(DIR_Interrupts_sources ${DIR_Interrupts_SRCS} ${DIR_Interrupts_hw_SRCS})
endif()



add_library(Interrupts ${DIR_Interrupts_sources})
target_include_directories(Interrupts PUBLIC include)

set(required_libraries pico_stdlib Common Buttons)

# add hw libs only if needed
if (NOT BUILD_MOCK_INTERRUPTS)
    # list(APPEND required_libraries I2C)
endif()

target_link_libraries(Interrupts PUBLIC ${required_libraries})
