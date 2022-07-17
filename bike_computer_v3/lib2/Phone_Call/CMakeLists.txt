# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_Phone_Call_SRCS 变量
aux_source_directory(. DIR_Phone_Call_SRCS)
include_directories(../Config)
include_directories(../../lib/Console)

# 生成链接库
add_library(Phone_Call ${DIR_Phone_Call_SRCS})
target_link_libraries(Phone_Call PUBLIC Config_GSM Console)