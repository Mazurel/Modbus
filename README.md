# Modbus
Modbus library for modern c++.

**WORK IN PROGRESS ...**

# Why
When I was working on my last project and tried to find a good c++ Modbus library (other than Qt) I was unable to find it.
That is why I have decided to share my own implementation of it.

## Important Concept
This library is **mainly** for providing Modbus logic, it doesnt aim to have best communiaction implementation.
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

# How to learn Modbus ?

Just use [Simply modbus](http://www.simplymodbus.ca/FAQ.htm).

# How to install it ?

### Using CMAKE

First go to directory that will contain this library.

``` bash
git clone https://github.com/Mazurel/Modbus
```

Then add to your CMakeLists.txt
```cmake
add_subdirectory(Modbus)
target_link_libraries(<your exec/lib> Modbus)
``` 

That is all !!!

# API

## Enums
- `MB::utils::MBErrorCode` - Enum that contains all the standard Modbus error Codes as well as Modbus library specific errors.


## Methods

## Classes

### ModbusResponse

- Constructors:
    - `static ModbusResponse::fromRaw(const std::vector<uint8_t>&)` - Creates ModbusResponse from raw bytes
    - `static ModbusResponse::fromRawCRC(const std::vector<uint8_t>&)` - Creates ModbusResponse from raw bytes and checks CRC. 
    When CRC is invalid throws InvalidCRC exception.
    - `ModbusResponse(uint8_t slaveId = 0, 
                      utils::MBFunctionCode functionCode = 0x00,
                      uint16_t address = 0, 
                      uint16_t registersNumber = 0,
                      std::vector<ModbusCell> values = {})` 
                      - Self explanatory constructor
- Methods:
    - `std::string toString()` - Returns ModbusResponse string representation.
    - `std::vector<uint8_t> toRaw()` - Converts ModbusResponse to vector of raw bytes.
    - `void from(const ModbusRequest&)` - Fills ModbusRequest with the request.
    Needed if you want ModbusResponse to have all the data.
    This method is needed because modbus protocol response doesn't have all the data.
- Getters:
- Setters:
