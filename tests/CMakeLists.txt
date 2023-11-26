project(Google_tests)

if (MSVC)
set(gtest_force_shared_crt on)
endif()

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ../include)

set(TestFiles MB/ModbusRequestTests.cpp
  MB/ModbusResponseTests.cpp
  MB/ModbusExceptionTests.cpp
  MB/ModbusCellTests.cpp
  main.cpp)

add_executable(Google_Tests_run ${TestFiles})

target_link_libraries(Google_Tests_run Modbus_Core)
target_link_libraries(Google_Tests_run gtest gtest_main)
