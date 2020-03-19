set(HEADER_FILES_DIR ${CMAKE_SOURCE_DIR}/include/)

# Include modbus core files
set(CORE_HEADER_FILES ${HEADER_FILES_DIR}/modbusCell.hpp
        ${HEADER_FILES_DIR}/modbusException.hpp
        ${HEADER_FILES_DIR}/modbusRequest.hpp
        ${HEADER_FILES_DIR}/modbusResponse.hpp
        ${HEADER_FILES_DIR}/modbusUtils.hpp)

set(CORE_SOURCE_FILES modbusException.cpp modbusRequest.cpp modbusResponse.cpp)

add_library(Modbus_Core)
target_sources(Modbus_Core PRIVATE ${CORE_SOURCE_FILES} PUBLIC ${CORE_HEADER_FILES})
target_include_directories(Modbus_Core PUBLIC ${HEADER_FILES_DIR})

add_library(Modbus)
target_link_libraries(Modbus Modbus_Core)


if(MODBUS_COMMUNICATION)
    message(Modbus communication is experimental)
    add_subdirectory(TCP)
    add_subdirectory(Serial)
    target_link_libraries(Modbus Modbus_TCP Modbus_Serial)
endif()
