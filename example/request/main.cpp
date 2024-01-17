#include <algorithm>
#include <iomanip>
#include <iostream>

#include "MB/ModbusException.hpp"
#include "MB/ModbusRequest.hpp"
#include "MB/ModbusResponse.hpp"

void create_request()
{
    // 创建一个简单的请求
    const MB::ModbusRequest request(1, MB::Utils::ReadDiscreteOutputCoils, 100, 10);

    std::cout << "Stringed Request: " << request.to_string() << '\n';

    std::cout << "Raw request:" << '\n';

    // 获得请求的原始表示
    std::vector<uint8_t> rawed = request.to_raw();

    // 打印字节的lambda函数
    auto show_byte = [](const uint8_t &byte) {
        std::cout << " 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    };

    // 显示所有的byte
    std::for_each(rawed.begin(), rawed.end(), show_byte);
    std::cout << '\n';

    // Create crc and pointer to its bytes
    uint16_t crc = MB::Utils::calculate_crc(rawed);
    auto *crc_ptr = reinterpret_cast<uint8_t *>(&crc);

    // Show byted crc for request
    std::cout << "crc for the above code: ";
    std::for_each(crc_ptr, crc_ptr + 2, show_byte);
    std::cout << '\n';

    auto request1 = MB::ModbusRequest::from_raw(rawed);
    std::cout << "Stringed Request 1 after rawed: " << request1.to_string() << '\n';

    // Add crc to the end of raw request so that it can be loaded with crc check
    rawed.insert(rawed.end(), crc_ptr, crc_ptr + 2);
    auto request2 = MB::ModbusRequest::from_raw_crc(rawed);  // Throws on invalid crc
    std::cout << "Stringed Request 2 after rawed: " << request2.to_string() << '\n';
}

int main()
{
    try {
        create_request();
    } catch (MB::ModbusException &ex) {
        std::cerr << ex.what() << '\n';
    }

    return 0;
}
