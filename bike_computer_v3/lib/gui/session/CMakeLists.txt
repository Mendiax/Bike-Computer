aux_source_directory(./source DIR_gui_session_SRCS)

include_directories(include)
include_directories(../common/include)




add_library(gui_session ${DIR_gui_session_SRCS})
target_link_libraries(gui_session PUBLIC
                      pico_multicore pico_stdlib
                      Tools Display Buttons Display Views gui_common Common
                      )
