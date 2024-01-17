# 递归添加头文件和源文件
file(GLOB_RECURSE SRCS *.cpp *.c *.cc *.cxx)
file(GLOB_RECURSE INCS *.hpp *.h)

target_sources(${PROJECT_NAME} PRIVATE ${INCS} ${SRCS})
