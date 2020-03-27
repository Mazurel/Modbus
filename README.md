<h1>Modbus library for modern c++</h1>

Modbus library for high level frame manipulation with modern c++17/20 with simple linux implemetation.

# Contents
- [Why](#why)
- [Important Concept](#important-concept)
- [Possibilities](#possibilities)
- [Status](#status)
- [Installation](#how-to-install-it-)
- [Api](#api)

# Why
When I was working on my last project and tried to find a good c++ Modbus library (other than Qt) I was unable to find it.
That is why I have decided to share my own implementation of it.

# Important Concept
This library is **mainly** for providing Modbus logic, it doesnt aim to have best communiaction implementation.
It gives user ability to create Modbus frames in high level api and convert them to raw bytes or show them as string.
That is why *Modbus Core* is OS independent and can be eaisly used with other communication frameworks.

It does have communiaction module which is **enabled** by default, and works pretty well on linux.

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

# STATUS

Currently Modbus Core is fully functional and (I belive) it doesn't have any bugs.

API for it is in progress.

Modbus Communication is working *currently* only for linux, it works well on TCP, not tested on Serial.

# How to learn Modbus ?

Just use [Simply modbus](http://www.simplymodbus.ca/FAQ.htm).

# How to install it ?

### Using CMAKE and git

First go to directory that will contain this library.

```bash
git clone https://github.com/Mazurel/Modbus
```

Then add to your CMakeLists.txt
```cmake
add_subdirectory(Modbus)
target_link_libraries(<your exec/lib> Modbus)
```
You should be able to use library.

**NOTE**
If you are on other os then gnu/linux you should disable communication part of modbus via cmake vriable MODBUS_COMMUNICATION.

# API (in progress)
- [Enums](#enums)
- [Methods](#methods)
- [Classes](#classes)

## Enums
Below each enum there are all values of enum.
- `MB::utils::MBErrorCode` - Enum that contains all the standard Modbus error Codes as well as Modbus library specific errors.
    ```c++
    // Documentation modbus errors:
    IllegalFunction = 0x01
    IllegalDataAddress = 0x02
    IllegalDataValue = 0x03
    SlaveDeviceFailure = 0x04
    Acknowledge = 0x05
    SlaveDeviceBusy = 0x06
    NegativeAcknowledge = 0x07
    MemoryParityError = 0x08
    GatewayPathUnavailable = 0x10
    GatewayTargetDeviceFailedToRespond = 0x11
  
    // Custom modbus errors:
    ErrorCodeCRCError = 0b0111111
    InvalidCRC = 0b01111110
    InvalidByteOrder = 0b01111101
    InvalidMessageID = 0b01111100
    ProtocolError = 0b01111011
    ConnectionClosed = 0b01111010
    Timeout = 0b01111001
    ```
- `MB::utils::MBFunctionCode` - Enum that contains all Modbus function codes.
    ```c++
    // Reading functions
    ReadDiscreteOutputCoils = 0x01
    ReadDiscreteInputContacts = 0x02
    ReadAnalogOutputHoldingRegisters = 0x03
    ReadAnalogInputRegisters = 0x04
    
    // Single write functions
    WriteSingleDiscreteOutputCoil = 0x05
    WriteSingleAnalogOutputRegister = 0x06
    
    // Multiple write functions
    WriteMultipleDiscreteOutputCoils = 0x0F
    WriteMultipleAnalogOutputHoldingRegisters = 0x10
    
    // Custom
    Undefined = 0x00
    ```
- `MB::utils::MBFunctionType` - Enum that contains function types.
    ```c++
    Read
    WriteSingle
    WriteMultiple
    ```
- `MB::utils::MBFunctionRegisters` - Enum that contains all register types.
    ```c++        
    OutputCoils
    InputContacts
    HoldingRegisters
    InputRegisters
    ```

## Methods

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

> For each getter and setter field there is:
>
> \<name\>() const - that gets the value
>
> set\<Name\>(value) - that sets value

#### ModbusException

Its prupose is to represent Modbus exception, either frame or c++ exception

- Constructor:
    - `ModbusException(const std::vector<uint8_t>& inputData, bool CRC = false);` - 
    Creates ModbusException from raw bytes, with CRC check based on parameter.
    - `ModbusException(utils::MBErrorCode errorCode, 
                       uint8_t slaveId = 0xFF, 
                       utils::MBFunctionCode functionCode = utils::Undefined)
                                                            noexcept :
                                                            _slaveId(slaveId),
                                                            _validSlave(true),
                                                            _errorCode(errorCode),
                                                            _functionCode(functionCode)
                                                            {}` - 
    Creates Modbus exception based on it's properties.
- Methods:
    - `static ModbusException::exist(const std::vector<uint8_t>& inputData)` -
    Checks if there is exception in modbus frame.
    - `std::string toString()` - 
    Returns string representation of exception.
    - `std::vector<uint8_t> toRaw()` -
    Retruns raw frame represenation of a excaption.
- Getters and setters:
    - functionCode
    - slaveID

#### ModbusRequest

Its purpose is to represent modbus request frame.

- Constructors:
    - `static ModbusRequest(std::vector<uint8_t> inputData, bool CRC = false)` -
    Creates Modbus request based on raw bytes and CRC boolean. If CRC is ON and the check fails constructor throws exception.
    - `static ModbusRequest::fromRaw(const std::vector<uint8_t>& inputData)` -
    Creates ModbusRequest from raw bytes.
    - `static ModbusRequest::fromRawCRC(const std::vector<uint8_t>& inputData)` - 
    Creates ModbusRequest from raw bytes and checks CRC.
    When CRC is invalid throws InvalidCRC exception.
    - `ModbusRequest(uint8_t slaveId = 0, 
                     utils::MBFunctionCode functionCode = static_cast<utils::MBFunctionCode>(0),
                     uint16_t address = 0, 
                     uint16_t registersNumber = 0,
                     std::vector<ModbusCell> values = {})` -
                     Self explanatory.
- Methods:
    - `std::string ModbusRequest::toString()` -
    Returns string representation of a request.
    - `std::vector<uint8_t> ModbusRequest::toRaw()` -
    Converts ModbusRequest to raw bytes.
    - `MB::utils::MBFunctionType functionType() const` - 
    Gets function type for current function code.
    - `MB::utils::MBFunctionRegisters functionRegisters() const` - 
    Gets function register for current function code.
- Getters and setters:
    - slaveID
    - functionCode
    - registerAddress
    - numberOfRegisters
    - registerValues

#### ModbusResponse

Its purpose is to represent response frame.

- Constructors:
    - `static ModbusResponse(std::vector<uint8_t> inputData, bool CRC = false)` -
    Creates Modbus response based on raw bytes and CRC boolean. If CRC is ON and the check fails constructor throws exception.
    - `static ModbusResponse::fromRaw(const std::vector<uint8_t>&)` -
    Creates ModbusResponse from raw bytes
    - `static ModbusResponse::fromRawCRC(const std::vector<uint8_t>&)` -
    Creates ModbusResponse from raw bytes and checks CRC. 
    When CRC is invalid throws InvalidCRC exception.
    - `ModbusResponse(uint8_t slaveId = 0, 
                      utils::MBFunctionCode functionCode = 0x00,
                      uint16_t address = 0, 
                      uint16_t registersNumber = 0,
                      std::vector<ModbusCell> values = {})` 
                      - 
                      Self explanatory constructor.
- Methods:
    - `std::string toString()` - 
    Returns ModbusResponse string representation.
    - `std::vector<uint8_t> toRaw()` -
    Converts ModbusResponse to vector of raw bytes.
    - `void from(const ModbusRequest&)` - 
    Fills ModbusResponse with the request.
    Needed if you want ModbusResponse to have all the data.
    This method is needed when you create object from raw.
    - `MB::utils::MBFunctionType functionType() const` - 
    Gets function type for current function code.
    - `MB::utils::MBFunctionRegisters functionRegisters() const` - 
    Gets function register for current function code.
- Getters and setters:
    - slaveID
    - functionCode
    - registerAddress
    - numberOfRegisters
    - registerValues

