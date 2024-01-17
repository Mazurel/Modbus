#include <iostream>

#include "MB/ModbusException.hpp"
#include "MB/ModbusRequest.hpp"
#include "MB/ModbusResponse.hpp"
#include "MB/TCP/Connection.hpp"
#include "MB/TCP/Server.hpp"

int main()
{
    std::cout << "modbus tcp slave\n";

    MB::TCP::Server slave(1150);

    auto conn = slave.await_connection();

    auto resp = conn->await_response();

    // modbus::Modbus mb;

    // mb.set_slave_id(1);

    return 0;
}
