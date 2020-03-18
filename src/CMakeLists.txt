set(HEADER_FILES_DIR ../include/)

set(HEADER_FILES ${HEADER_FILES_DIR}/modbusCell.hpp
        ${HEADER_FILES_DIR}/modbusException.hpp
        ${HEADER_FILES_DIR}/modbusRequest.hpp
        ${HEADER_FILES_DIR}/modbusResponse.hpp
        ${HEADER_FILES_DIR}/modbusUtils.hpp)

set(SOURCE_FILES modbusException.cpp modbusRequest.cpp modbusResponse.cpp)

add_library(Modbus)

target_sources(Modbus PRIVATE ${SOURCE_FILES} PUBLIC ${HEADER_FILES} )
target_include_directories(Modbus PUBLIC ${HEADER_FILES_DIR})

#add_subdirectory(TCP)
#add_subdirectory(Serial)