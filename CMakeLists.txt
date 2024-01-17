cmake_minimum_required(VERSION 3.13)
project(protocolConverter)

set(CMAKE_CXX_STANDARD 17)

if(DCMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -lpthread -Wall")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/utf-8)
endif()

option(MODBUS_EXAMPLE "Build example program" ON)
option(MODBUS_TESTS "Build tests" OFF)
option(MODBUS_COMMUNICATION "Use Modbus communication library" ON)

add_subdirectory(src)

if(MODBUS_TESTS)
  add_subdirectory(tests)
endif()

if(MODBUS_EXAMPLE)
  add_subdirectory(example)
endif()
