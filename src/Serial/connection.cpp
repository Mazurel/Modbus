// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "Serial/connection.hpp"

using namespace MB::Serial;

Connection::Connection(const std::string& path)
{
    _fd = open(path.c_str(), O_RDWR | O_SYNC);

    if (_fd < 0)
    {
        throw std::runtime_error("Cannot open serial port " + path);
    }

    _termios = {};
    if (tcgetattr(_fd , &_termios) != 0)
    {
        throw std::runtime_error("Error at tcgetattr - " + std::to_string(errno));
    }

    getTTY().c_iflag &= ~ECHO;
}

void Connection::connect()
{
    tcflush(_fd, TCIFLUSH);
    if (tcsetattr(_fd , TCSANOW, &_termios) != 0)
    {
        throw std::runtime_error("Error at tcsetattr - " + std::to_string(errno));
    }
}

Connection::~Connection()
{
    if (_fd >= 0)
        ::close(_fd);
}

void Connection::sendRequest(const MB::ModbusRequest &request)
{
    send(request.toRaw());
}

void Connection::sendResponse(const MB::ModbusResponse &response)
{
    send(response.toRaw());
}

void Connection::sendException(const MB::ModbusException &exception)
{
    send(exception.toRaw());
}

MB::ModbusResponse Connection::awaitResponse()
{
    std::vector<uint8_t> data(1024);

    pollfd waitingFD = { .fd = _fd, .events = POLLIN, .revents = POLLIN};

    if (::poll(&waitingFD , 1 , _timeout) <= 0)
    {
        throw MB::ModbusException(MB::utils::Timeout);
    }

    auto size = ::read(_fd, data.begin().base() , 1024);

    if (size < 0)
    {
        throw MB::ModbusException(MB::utils::SlaveDeviceFailure);
    }

    data.resize(size);
    data.shrink_to_fit();

    if (MB::ModbusException::exist(data))
    {
        throw MB::ModbusException(data);
    }

    return MB::ModbusResponse::fromRawCRC(data);
}

MB::ModbusRequest Connection::awaitRequest()
{
    std::vector<uint8_t> data(1024);

    auto size = ::read(_fd, data.begin().base() , 1024);

    if (size < 0)
    {
        throw MB::ModbusException(MB::utils::SlaveDeviceFailure);
    }

    data.resize(size);
    data.shrink_to_fit();

    if (MB::ModbusException::exist(data))
    {
        throw MB::ModbusException(data);
    }

    return MB::ModbusRequest::fromRawCRC(data);
}

void Connection::send(std::vector<uint8_t> &&data)
{
    data.reserve(data.size() + 2);
    const auto crc = utils::calculateCRC(data.begin().base(), data.size());

    data.push_back(reinterpret_cast<const uint8_t *>(&crc)[0]);
    data.push_back(reinterpret_cast<const uint8_t *>(&crc)[1]);

    ::write(_fd , data.begin().base(), data.size());
}
