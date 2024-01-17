#include <algorithm>
#include <iomanip>
#include <iostream>

#include "MB/ModbusException.h"
#include "MB/ModbusRequest.h"
#include "MB/ModbusResponse.h"
#include "MB/TCP/Connection.h"
#include "MB/TCP/Server.h"

#define PORT 1150

int main()
{
    MB::TCP::Server slave(PORT);

    std::vector<MB::ModbusCell> cell(18);
    cell[0] = MB::ModbusCell::init_reg(1);
    cell[1] = MB::ModbusCell::init_reg(2);
    cell[2] = MB::ModbusCell::init_reg(3);
    cell[3] = MB::ModbusCell::init_reg(4);
    cell[4] = MB::ModbusCell::init_reg(5);
    cell[5] = MB::ModbusCell::init_reg(6);
    cell[6] = MB::ModbusCell::init_reg(7);
    cell[17] = MB::ModbusCell::init_reg(0xFFFF);

    MB::ModbusResponse resp(1, MB::Utils::ReadAnalogOutputHoldingRegisters, 0x1F0, 18, cell);

    while (true) {
        auto conn = slave.await_connection();
        if (!conn) {
            continue;
        }

        while (true) {
            try {
                auto req = conn->await_request();

                std::cout << "slave_id: " << (int)req.slave_id() << '\n';
                std::cout << "function_code: " << (int)req.function_code() << '\n';
                std::cout << "register_address: " << req.register_address() << '\n';
                std::cout << "number_of_registers: " << req.number_of_registers() << '\n';

                if (req.slave_id() != 1) {
                    continue;
                }

                if (req.function_code() != MB::Utils::ReadAnalogOutputHoldingRegisters) {
                    throw MB::ModbusException(MB::Utils::MBErrorCode::IllegalFunction);
                }

                if (req.register_address() != 0x1F0) {
                    throw MB::ModbusException(MB::Utils::MBErrorCode::IllegalDataAddress);
                }

                if (req.number_of_registers() != 18) {
                    throw MB::ModbusException(MB::Utils::MBErrorCode::IllegalDataValue);
                }

                conn->send_response(resp);

            } catch (MB::ModbusException &ex) {
                std::cout << ex.what() << '\n';

                if (ex.get_error_code() == MB::Utils::ConnectionClosed) {
                    break;
                }

                conn->send_exception(ex);
            }
        }
    }

    return EXIT_SUCCESS;
}
