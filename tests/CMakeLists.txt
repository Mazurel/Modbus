project(Google_tests)

if (MSVC)
set(CMAKE_CXX_STANDARD 20)
set(gtest_force_shared_crt on)
endif()

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(TestFiles MB/ModbusRequestTests.cpp
  MB/ModbusResponseTests.cpp
  MB/ModbusExceptionTests.cpp
  MB/ModbusCellTests.cpp
  main.cpp)

add_executable(Google_Tests_run ${TestFiles})

if (MSVC)
target_link_libraries(Google_Tests_run Modbus_Core ws2_32.lib Iphlpapi.lib)
else()
target_link_libraries(Google_Tests_run Modbus_Core)
endif()
target_link_libraries(Google_Tests_run gtest gtest_main)
