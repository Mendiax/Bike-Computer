# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_AT_TEST_SRCS 变量
aux_source_directory(. DIR_AT_TEST_SRCS)
include_directories(../Config)
include_directories(../../lib/Console)

# 生成链接库
add_library(AT_TEST ${DIR_AT_TEST_SRCS})
target_link_libraries(AT_TEST PUBLIC Config_GSM Console)