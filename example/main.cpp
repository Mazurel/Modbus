// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "MB/modbusException.hpp"
#include "MB/modbusRequest.hpp"
#include "MB/modbusResponse.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>

void createRequest() {
    // Create simple request
    MB::ModbusRequest request(1, MB::utils::ReadDiscreteOutputCoils, 100, 10);

    std::cout << "Stringed Request: " << request.toString() << std::endl;

    std::cout << "Raw request:" << std::endl;

    // Get raw represenatation for request
    std::vector<uint8_t> rawed = request.toRaw();

    // Method for showing byte
    auto showByte = [](const uint8_t &byte) {
        std::cout << " 0x" << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(byte);
    };

    // Show all bytes
    std::for_each(rawed.begin(), rawed.end(), showByte);
    std::cout << std::endl;

    // Create CRC and pointer to its bytes
    uint16_t CRC = MB::utils::calculateCRC(rawed);
    auto CRCptr  = reinterpret_cast<uint8_t *>(&CRC);

    // Show byted CRC for request
    std::cout << "CRC for the above code: ";
    std::for_each(CRCptr, CRCptr + 2, showByte);
    std::cout << std::endl;

    auto request1 = MB::ModbusRequest::fromRaw(rawed);
    std::cout << "Stringed Request 1 after rawed: " << request1.toString() << std::endl;

    // Add CRC to the end of raw request so that it can be loaded with CRC check
    rawed.insert(rawed.end(), CRCptr, CRCptr + 2);
    auto request2 = MB::ModbusRequest::fromRawCRC(rawed); // Throws on invalid CRC
    std::cout << "Stringed Request 2 after rawed: " << request2.toString() << std::endl;
}

int main() { createRequest(); }
