set(MODBUS_SERIAL_HEADER_FILES ${HEADER_FILES_DIR}/Serial/connection.hpp)
set(MODBUS_SERIAL_SOURCE_FILES connection.cpp)

add_library(Modbus_Serial)
target_include_directories(Modbus_Serial PRIVATE ${HEADER_FILES_DIR})
target_link_libraries(Modbus_Serial Modbus_Core)
target_sources(Modbus_Serial PRIVATE ${MODBUS_SERIAL_SOURCE_FILES} PUBLIC ${MODBUS_SERIAL_HEADER_FILES})