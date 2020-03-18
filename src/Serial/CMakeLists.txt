set(HEADER_FILES_DIR ../../../include/MB/Serial)

set(HEADER_FILES ${HEADER_FILES_DIR}/connection.hpp)
set(SOURCE_FILES connection.cpp)

target_sources(Modbus PRIVATE ${SOURCE_FILES} PUBLIC ${HEADER_FILES})