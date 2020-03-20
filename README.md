<h1> Modbus library for modern c++</h1>

**WORK IN PROGRESS ...**

- [Why](#why)
- [Important Concept](#important-concept)
- [Quick Example](#quick-example)
- [Status](#status)
- [Installation](#how-to-install-it-?)
- [Api](#api)

#Why
When I was working on my last project and tried to find a good c++ Modbus library (other than Qt) I was unable to find it.
That is why I have decided to share my own implementation of it.

##Important Concept
This library is **mainly** for providing Modbus logic, it doesnt aim to have best communiaction implementation.
It gives user ability to create Modbus frames in high level api and convert them to raw bytes or show them as string.
That is why *Modbus Core* is OS independent and can be used with many great librarys that are good at transporting data.

#Quick Example

Quick example of what Modbus Core can do:

Code:
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
Output:
```bash
Stringed Request: Read from output coils, from slave 1, starting from address 100, on 10 registers
Raw request:
 01 01 00 64 00 0a
Stringed Request 1 after rawed: Read from output coils, from slave 1, starting from address 100, on 10 registers
Stringed Request 2 after rawed: Read from output coils, from slave 1, starting from address 100, on 10 registers
```
 
*More examples are in the examples directory*

# STATUS

Currently Modbus Core is fully functional and (I belive) it doesn't have bugs.

Modbus Communication is working *currently* only for linux, it works well on TCP, not tested on Serial.

# How to learn Modbus ?

Just use [Simply modbus](http://www.simplymodbus.ca/FAQ.htm).

# How to install it ?

### Using CMAKE and git

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
Below each enum there are all values of enum.
> modbusutils.hpp
- `MB::utils::MBErrorCode` - Enum that contains all the standard Modbus error Codes as well as Modbus library specific errors.
    ```c++
    // Documentation modbus errors:
    IllegalFunction = 0x01,
    IllegalDataAddress = 0x02,
    IllegalDataValue = 0x03,
    SlaveDeviceFailure = 0x04,
    Acknowledge = 0x05,
    SlaveDeviceBusy = 0x06,
    NegativeAcknowledge = 0x07,
    MemoryParityError = 0x08,
    GatewayPathUnavailable = 0x10,
    GatewayTargetDeviceFailedToRespond = 0x11,
  
    // Custom modbus errors:
    ErrorCodeCRCError = 0b0111111,
    InvalidCRC = 0b01111110,
    InvalidByteOrder = 0b01111101,
    InvalidMessageID = 0b01111100,
    ProtocolError = 0b01111011,
    ConnectionClosed = 0b01111010,
    Timeout = 0b01111001
    ```
- `MB::utils::MBFunctionCode` - Enum that contains all Modbus function codes.
    ```c++
    // Reading functions
    ReadDiscreteOutputCoils = 0x01,
    ReadDiscreteInputContacts = 0x02,
    ReadAnalogOutputHoldingRegisters = 0x03,
    ReadAnalogInputRegisters = 0x04,
    
    // Single write functions
    WriteSingleDiscreteOutputCoil = 0x05,
    WriteSingleAnalogOutputRegister = 0x06,
    
    // Multiple write functions
    WriteMultipleDiscreteOutputCoils = 0x0F,
    WriteMultipleAnalogOutputHoldingRegisters = 0x10,
    
    // Custom
    Undefined = 0x00
    ```
- `MB::utils::MBFunctionType` - Enum that contains function types.
    ```c++
    Read,
    WriteSingle,
    WriteMultiple
    ```
- `MB::utils::MBFunctionRegisters` - Enum that contains all register types.
    ```c++        
    OutputCoils,
    InputContacts,
    HoldingRegisters,
    InputRegisters
    ```

## Methods

> modbusutils.hpp

- `bool MB::utils::isStandardErrorCode(MBErrorCode code)` - 
    Returns true if specified code is a Modbus standard error code.
- `std::string MB::utils::mbErrorCodeToStr(MBErrorCode code)` -
    Returns stringed name of a specified Modbus error code.  
- `MBFunctionType MB::utils::functionType(const MBFunctionCode code)` - 
    Get functions type based on function code.
- `MBFunctionRegisters MB::utils::functionRegister(const MBFunctionCode code)` - 
    Get functions register based on function code.
- `uint16_t MB::utils::bigEndianConv(const uint8_t *buf)` -
    Creates uint16_t number from uint8_t buffer of two bytes (used when reading modbus frames).
- `uint16_t MB::utils::calculateCRC(const uint8_t *buff, size_t len)`

  `uint16_t MB::utils::calculateCRC(const std::vector<uint8_t>& buffer)` - 
  Pretty self explanatory.
## Classes

> modbusResponse.hpp
#### ModbusResponse

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
    - `void from(const ModbusRequest&)` - Fills ModbusResponse with the request.
    Needed if you want ModbusResponse to have all the data.
    This method is needed when you create object from raw.
- Getters:
- Setters:
