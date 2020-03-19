set(MODBUS_TCP_HEADER_FILES ${HEADER_FILES_DIR}/TCP/connection.hpp
        ${HEADER_FILES_DIR}/TCP/server.hpp)

set(MODBUS_TCP_SOURCE_FILES connection.cpp server.cpp)

add_library(Modbus_TCP)
target_include_directories(Modbus_TCP PRIVATE ${HEADER_FILES_DIR})
target_link_libraries(Modbus_TCP Modbus_Core)
target_sources(Modbus_TCP PRIVATE ${MODBUS_TCP_SOURCE_FILES} PUBLIC ${MODBUS_TCP_HEADER_FILES})