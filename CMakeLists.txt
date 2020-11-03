cmake_minimum_required(VERSION 3.13)
project(protocolConverter)

set(CMAKE_CXX_STANDARD 20)

if (DCMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -lpthread -Wall")
endif()

option(MODBUS_EXAMPLE "Build example program" OFF)
option(MODBUS_TESTS "Build tests" OFF)
option(MODBUS_COMMUNICATION "Use Modbus communication library" ON)

add_subdirectory(src)

if(MODBUS_TESTS)
  add_subdirectory(tests)
endif()

if(MODBUS_EXAMPLE)
    add_executable(ex example/main.cpp)
    target_link_libraries(ex Modbus)
endif()

