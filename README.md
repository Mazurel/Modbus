<h1>Modbus library for modern C++</h1>

Library for high level Modbus frame manipulation, including encoding and decoding, written in modern C++17.

Additionally, library contains simple linux TCP and RTU implementation,
that you can use as a starting point for you application !

# Table of content
- [Why](#why)
- [Important Concept](#important-concept)
- [Possibilities](#possibilities)
- [Dependencies](#dependencies)
- [Status](#status)
- [Installation](#how-to-install-it-)
- [Api](#api)

# Why

When I was working on my last project and tried to find a good C++ Modbus library (other than Qt) I was unable to find it.
That is why I have decided to share my own implementation of Modbus.

# Important Concept

This library is **mainly** for providing Modbus logic.
It doesnt aim to have best communiaction implementation, as it is usually HW-specific.
It gives user ability to create Modbus frames using high level api and convert them to raw bytes or show them as string.
That is why *Modbus Core* is OS independent and can be easily used with other communication frameworks,
assuming that you compiler supports at least C++17.

Modbus communiaction module is **enabled** by default and works pretty well on linux.
If you are using your own communication, be sure to disable it via cmake variable.

# Possibilities

Quick example of what Modbus Core can do:

Code:
```c++
#include <modbusRequest.hpp>

// Create simple request
MB::ModbusRequest request(1, MB::utils::ReadDiscreteOutputCoils, 100, 10);

std::cout << "Stringed Request: " << request.toString() << std::endl;

std::cout << "Raw request:" << std::endl;

// Get raw represenatation for request
std::vector<uint8_t> rawed = request.toRaw();

// Method for showing byte
auto showByte = [](const uint8_t& byte)
{
    std::cout << " 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
};

// Show all bytes
std::for_each(rawed.begin(), rawed.end(), showByte);
std::cout << std::endl;

// Create CRC and pointer to its bytes
uint16_t CRC = MB::utils::calculateCRC(rawed);
auto CRCptr = reinterpret_cast<uint8_t *>(&CRC);

// Show byted CRC for request
std::cout << "CRC for the above code: ";
std::for_each(CRCptr, CRCptr + 2, showByte);
std::cout << std::endl;


auto request1 = MB::ModbusRequest::fromRaw(rawed);
std::cout << "Stringed Request 1 after rawed: " << request1.toString() << std::endl;

// Add CRC to the end of raw request so that it can be loaded with CRC check
rawed.insert(rawed.end() , CRCptr, CRCptr + 2);
auto request2 = MB::ModbusRequest::fromRawCRC(rawed); // Throws on invalid CRC
std::cout << "Stringed Request 2 after rawed: " << request2.toString() << std::endl;
```
Output:
```bash
Stringed Request: Read from output coils, from slave 1, starting from address 100, on 10 registers
Raw request:
 0x01 0x01 0x00 0x64 0x00 0x0a
CRC for the above code:  0xfd 0xd2
Stringed Request 1 after rawed: Read from output coils, from slave 1, starting from address 100, on 10 registers
Stringed Request 2 after rawed: Read from output coils, from slave 1, starting from address 100, on 10 registers
```

# Dependencies

Library core is dependency free - except for C++ standard library.

For communication module, you may need:

- libnet - only for tcp communication (not needed if communication is disabled)

# STATUS

Currently Modbus Core is fully functional and (I belive) it doesn't have any bugs.

Modbus Communication is working *currently* only for linux, it works well on TCP and Serial (tested on raspberry pi).

# How to learn Modbus ?

Just use [Simply modbus](http://www.simplymodbus.ca/FAQ.htm).

# How to install it ?

### Using CMAKE and git

First go to directory that will contain this library.

```bash
git clone https://github.com/Mazurel/Modbus
git submodule update --init --recursive # Fetch submodules (google tests)
```

Then add to your CMakeLists.txt
```cmake
add_subdirectory(Modbus)
target_link_libraries(<your exec/lib> Modbus)
```
You should be able to use library.

**NOTE**
If you are on other os then gnu/linux you should disable communication part of modbus via cmake variable MODBUS_COMMUNICATION.

# API

API documentation is generated using [Doxygen](https://www.doxygen.nl) and it is available online under this [link](https://mazurel.github.io/docs/modbus/index.html).
If you want, you can also generate it yourself !
