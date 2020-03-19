project(Google_tests)
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(TestFiles MB/ModbusRequestTests.cpp MB/ModbusResponseTests.cpp MB/ModbusExceptionTests.cpp main.cpp)

add_executable(Google_Tests_run ${TestFiles})

target_link_libraries(Google_Tests_run Modbus)
target_link_libraries(Google_Tests_run gtest gtest_main)