//
// Created by mateusz on 18.03.2020.
//

#include "modbusResponse.hpp"
#include "modbusRequest.hpp"
#include "modbusException.hpp"

#include <iostream>
#include <iomanip>

void createRequest()
{
    MB::ModbusRequest request(1 ,
                          MB::utils::ReadDiscreteOutputCoils,
                              100,
                            10);

    std::cout << "Stringed Request: " << request.toString() << std::endl;
    std::cout << "Raw request:" << std::endl;

    auto rawed = request.toRaw(); // simple std::vector
    for (const auto& byte: rawed)
    {
        std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    std::cout << std::endl;

    auto request1 = MB::ModbusRequest::fromRaw(rawed);
    std::cout << "Stringed Request 1 after rawed: " << request1.toString() << std::endl;

    uint16_t CRC = MB::utils::calculateCRC(rawed.begin().base(), rawed.size());
    auto CRCptr = reinterpret_cast<uint8_t *>(&CRC);
    rawed.insert(rawed.end() , CRCptr, CRCptr + 2);

    auto request2 = MB::ModbusRequest::fromRawCRC(rawed); // Throws on invalid CRC
    std::cout << "Stringed Request 2 after rawed: " << request2.toString() << std::endl;
}


int main()
{
    createRequest();
}