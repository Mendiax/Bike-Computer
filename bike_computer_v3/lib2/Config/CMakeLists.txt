# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_Config_SRCS 变量
aux_source_directory(. DIR_Config_GSM_SRCS)

include_directories(../../lib/Console)

# 生成链接库
add_library(Config_GSM ${DIR_Config_GSM_SRCS})
target_link_libraries(Config_GSM PUBLIC pico_stdlib hardware_adc Console)