# Modbus
Modbus library for modern c++.

**WORK IN PROGRESS ...**

# Why
When I was working on my last project and tried to find a good c++ Modbus library (other than Qt) I was unable to find it.
That is why I have decided to share my own implementation of it.

## Important Concept
This library is **mainly** for prowiding modbus logic, it doesnt aim to have best communiaction implementation.
It gives user ability to create Modbus frames in high level api and convert them to raw bytes or show them as string.
That is why *Modbus Core* is OS independent and can be used with many great librarys that are good at transporting data.

# Examples

Simple example of what Modbus core can do:

```c++
#include <modbusRequest.hpp>

MB::ModbusRequest request(
                      1 , // Slave ID
                      MB::utils::ReadDiscreteOutputCoils, // Function Code
                      100, // Address
                      10   // Number of registers
                      );

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
```

# STATUS

Currently Modbus Core is fully functional and (I belive) it doesn't have bugs.

Modbus Communication is working *currently* only for linux, it works well on TCP, not tested on Serial.

# API

To be added ...

