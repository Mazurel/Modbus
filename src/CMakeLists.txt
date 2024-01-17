set(
  MODBUS_HEADER_FILES_DIR
  ${PROJECT_SOURCE_DIR}/include/MB
)

# Include modbus core files
set(
  CORE_HEADER_FILES
  ${MODBUS_HEADER_FILES_DIR}/ModbusCell.hpp
  ${MODBUS_HEADER_FILES_DIR}/ModbusException.hpp
  ${MODBUS_HEADER_FILES_DIR}/ModbusRequest.hpp
  ${MODBUS_HEADER_FILES_DIR}/ModbusResponse.hpp
  ${MODBUS_HEADER_FILES_DIR}/ModbusUtils.hpp
)

set(
  CORE_SOURCE_FILES
  ModbusException.cpp
  ModbusRequest.cpp
  ModbusResponse.cpp
)

add_library(Modbus_Core)

target_sources(
  Modbus_Core
  PRIVATE ${CORE_SOURCE_FILES}
  PUBLIC ${CORE_HEADER_FILES}
)

target_include_directories(
  Modbus_Core
  PUBLIC ${PROJECT_SOURCE_DIR}/include
  PRIVATE ${MODBUS_HEADER_FILES_DIR}
)

add_library(Modbus)
target_link_libraries(
  Modbus
  Modbus_Core
)

if(MODBUS_COMMUNICATION)
  message("Modbus communication is experimental")
  add_subdirectory(TCP)
  target_link_libraries(Modbus Modbus_TCP)
endif()
