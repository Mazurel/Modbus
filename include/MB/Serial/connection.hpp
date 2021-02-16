// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#pragma once

#include <string>
#include <stdexcept>
#include <sstream>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cerrno>
#include <poll.h>

#include "MB/modbusUtils.hpp"
#include "MB/modbusResponse.hpp"
#include "MB/modbusRequest.hpp"
#include "MB/modbusException.hpp"

namespace MB::Serial
{
class Connection
{
public:
    static const unsigned int DefaultSerialTimeout = 1000;

private:
    struct termios _termios;
    int _fd;

    int _timeout = Connection::DefaultSerialTimeout;

public:
    constexpr explicit Connection() : _termios(), _fd(-1) {}
    explicit Connection(const std::string& path);
    explicit Connection(const Connection&) = delete;
    explicit Connection(Connection&&) noexcept;
    Connection& operator=(Connection&&);
    ~Connection();

    void connect();


    void sendRequest(const MB::ModbusRequest& request);
    void sendResponse(const MB::ModbusResponse& response);
    void sendException(const MB::ModbusException& exception);

    /**
     * @brief Sends data through the serial
     * @param data - Vectorized data
     */
    void send(std::vector<uint8_t> data);

    void clearInput();

    [[nodiscard]] MB::ModbusResponse awaitResponse();
    [[nodiscard]] MB::ModbusRequest awaitRequest();

    [[nodiscard]] std::vector<uint8_t> awaitRawMessage();

    void enableParity(const bool parity)
    {
        if (parity)
            getTTY().c_cflag |= PARENB;
        else
            getTTY().c_cflag &= ~PARENB;
    }

    void setEvenParity()
    {
        enableParity(true);
        getTTY().c_cflag &= ~PARODD;
    }

    void setOddParity()
    {
        enableParity(true);
        getTTY().c_cflag |= PARODD;
    }

    void setTwoStopBits(const bool two)
    {
        if (two)
        {
            getTTY().c_cflag |= CSTOPB;
        }
        else
        {
            getTTY().c_cflag &= ~CSTOPB;
        }
    }


#define setBaud(s) case s: speed = B ## s; break;
    void setBaudRate(speed_t speed)
    {
        switch (speed)
        {
            setBaud(0);
            setBaud(50);
            setBaud(75);
            setBaud(110);
            setBaud(134);
            setBaud(150);
            setBaud(200);
            setBaud(300);
            setBaud(600);
            setBaud(1200);
            setBaud(1800);
            setBaud(2400);
            setBaud(4800);
            setBaud(9600);
            setBaud(19200);
            setBaud(38400);
            setBaud(57600);
            setBaud(115200);
            setBaud(230400);
            default:
                throw std::runtime_error("Invalid baud rate");
        }
        cfsetospeed(&_termios , speed);
        cfsetispeed(&_termios , speed);
    }
#undef setBaud

    termios & getTTY()
    {
        return _termios;
    }

    int getTimeout() const
    {
        return _timeout;
    }

    void setTimeout(int timeout)
    {
        _timeout = timeout;
    }
};
}
