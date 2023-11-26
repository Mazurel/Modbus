set(MODBUS_HEADER_FILES_DIR ${PROJECT_SOURCE_DIR}/include/MB)

# Include modbus core files
set(CORE_HEADER_FILES ${MODBUS_HEADER_FILES_DIR}/modbusCell.hpp
        ${MODBUS_HEADER_FILES_DIR}/modbusException.hpp
        ${MODBUS_HEADER_FILES_DIR}/modbusRequest.hpp
        ${MODBUS_HEADER_FILES_DIR}/modbusResponse.hpp
        ${MODBUS_HEADER_FILES_DIR}/modbusUtils.hpp) 

set(CORE_SOURCE_FILES modbusException.cpp
  modbusRequest.cpp
  modbusResponse.cpp)

add_library(Modbus_Core)
target_sources(Modbus_Core PRIVATE ${CORE_SOURCE_FILES} PUBLIC ${CORE_HEADER_FILES})
target_include_directories(Modbus_Core PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${MODBUS_HEADER_FILES_DIR})

add_library(Modbus)
target_link_libraries(Modbus Modbus_Core)


if(MODBUS_TCP_COMMUNICATION)
    message("Modbus TCP communication is experimental")
    add_subdirectory(TCP)
    target_link_libraries(Modbus Modbus_TCP)
endif()

if(MODBUS_SERIAL_COMMUNICATION)
    message("Modbus serial communication is experimental")
    add_subdirectory(Serial)
    target_link_libraries(Modbus Modbus_Serial)
endif()
