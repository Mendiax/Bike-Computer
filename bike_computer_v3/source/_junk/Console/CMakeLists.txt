aux_source_directory(. DIR_Console_SRCS)

include_directories(../../drivers/display/include)


add_library(Console ${DIR_Console_SRCS})
target_link_libraries(Console PUBLIC Display pico_stdlib hardware_spi hardware_i2c hardware_pwm hardware_adc)