project(request)

message(STATUS "[example] 项目名称 ${PROJECT_NAME}")

file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE INCS *.hpp *.h)

add_executable(${PROJECT_NAME} ${SRCS} ${INCS})

target_link_libraries(${PROJECT_NAME} modbus)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  PRIVATE $<BUILD_INTERFACE:${modbus_SOURCE_DIR}/include>
)

# 设置编译器选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()
