cmake_minimum_required(VERSION 3.15)
project(protocolConverter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -lpthread -Wall")

option(MODBUS_EXAMPLE "Run example program" ON)
option(MODBUS_TEST "Run tests" ON)

add_subdirectory(src)

if(MODBUS_EXAMPLE)
    add_executable(ex example/main.cpp)
    target_link_libraries(ex Modbus)
endif()

if(MODBUS_TEST)
    add_subdirectory(Google_tests)
endif()