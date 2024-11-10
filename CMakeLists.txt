cmake_minimum_required(VERSION 3.13)
project(modbus)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_compile_options(/W3 /MT)
    # Otherwise, MSVC complains about strncopy
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

option(MODBUS_EXAMPLE "Build example program" OFF)
option(MODBUS_TESTS "Build tests" OFF)
option(MODBUS_TCP_COMMUNICATION "Use Modbus TCP communication library" ON)

if(NOT win32)
    # Serial not supported on Windows
    option(MODBUS_SERIAL_COMMUNICATION "Use Modbus serial communication library" OFF)  # not supported by windows platform
else()
    message(STATUS "Modbus Serial not supported on Windows.")
endif()

add_subdirectory(src)

if(MODBUS_TESTS)
  add_subdirectory(tests)
endif()

if(MODBUS_EXAMPLE)
    add_executable(ex example/main.cpp)
    target_link_libraries(ex PUBLIC Modbus_Core)
endif()
