set(HEADER_FILES_DIR ../../../include/MB/TCP)

set(HEADER_FILES ${HEADER_FILES_DIR}/connection.hpp
        ${HEADER_FILES_DIR}/server.hpp)

set(SOURCE_FILES connection.cpp server.cpp)

target_sources(Modbus PRIVATE ${SOURCE_FILES} PUBLIC ${HEADER_FILES})