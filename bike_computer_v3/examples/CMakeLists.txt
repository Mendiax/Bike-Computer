# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_examples_SRCS 变量
aux_source_directory(. DIR_examples_SRCS)

include_directories(../lib2/Config)
include_directories(../lib2/Infrared)
include_directories(../lib2/AT_TEST)
include_directories(../lib2/BT)
include_directories(../lib2/GPS)
include_directories(../lib2/HTTP)
include_directories(../lib2/Phone_Call)
include_directories(../lib2/SMS)

# 生成链接库
add_library(examples ${DIR_examples_SRCS})
target_link_libraries(examples PUBLIC
                    Config_GSM Infrared_GSM AT_TEST BT GPS HTTP Phone_Call SMS)