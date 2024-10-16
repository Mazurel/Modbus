project(Google_tests)
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

if (MSVC)
    set(gtest_force_shared_crt on)
    add_compile_options(/MT)
endif()

set(TestFiles MB/ModbusRequestTests.cpp
  MB/ModbusResponseTests.cpp
  MB/ModbusExceptionTests.cpp
  MB/ModbusCellTests.cpp
  MB/ModbusFunctionalTests.cpp
  main.cpp)

add_executable(Google_Tests_run ${TestFiles})

target_link_libraries(Google_Tests_run Modbus_Core)
target_link_libraries(Google_Tests_run gtest gtest_main)
