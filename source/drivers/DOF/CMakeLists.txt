aux_source_directory(./src DIR_DOF_SRCS)
aux_source_directory(./src_hw DIR_DOF_hw_SRCS)
aux_source_directory(./src_mock DIR_DOF_mock_SRCS)

if (BUILD_MOCK_DOF)
    set(DIR_dof_sources ${DIR_DOF_SRCS} ${DIR_DOF_mock_SRCS})
else()
    set(DIR_dof_sources ${DIR_DOF_SRCS} ${DIR_DOF_hw_SRCS})
endif()



add_library(DOF ${DIR_dof_sources})
target_include_directories(DOF PUBLIC include)

set(required_libraries pico_stdlib Tools Common Filters Interrupts)

# add hw libs only if needed
if (NOT BUILD_MOCK_DOF)
    list(APPEND required_libraries I2C)
endif()

target_link_libraries(DOF PUBLIC ${required_libraries})
